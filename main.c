/* TODO setsockopt() make unix_socket nonblocking */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include "headers/config.h"
#include "headers/user_interaction.h"
#include "headers/network.h"
#include "headers/startup.h"

/* Init global variables */
struct irc_server irc_servers[MAX_IRC_SERVERS] = {0};
struct bot_command_request bot_command_requests[25] = {0};
unsigned char irc_servers_index = 0;
unsigned char bot_command_requests_index = 0;
char quit_prompt_input[QUIT_PROMPT_RESULT_SIZE];
char yes_no_prompt[YES_NO_PROMPT_SIZE];
fd_set connections, set_tmp;
unsigned char program_mode;
int unix_socket;
int message_on_unix_socket_buffer_index = 0;
int current_unix_socket;
unsigned char current_unix_socket_connected = 0;
unsigned char unix_socket_connected = 0;
struct sockaddr_un unix_socket_address;
int highest_fd = 10;
socklen_t unix_socket_len;
char message_on_unix_socket_buffer[MESSAGE_ON_UNIX_SOCKET_BUFFER_SIZE];
char message_from_unix_socket_buffer[MESSAGE_FROM_UNIX_SOCKET_BUFFER_SIZE];

void exit_entire_program(int exit_status) {
		printf("exiting...\n");
		exit(exit_status);
}

int main(int argc, char *argv[])
{
	
	/* Declare local variables */
	int retval1;
	int pid = 1;
	
	/* Initialize variables */
	/* Add null bytes to end of each string for safety */
	quit_prompt_input[QUIT_PROMPT_RESULT_SIZE - 1] = 0;
	yes_no_prompt[YES_NO_PROMPT_SIZE - 1] = 0;
	message_on_unix_socket_buffer[MESSAGE_ON_UNIX_SOCKET_BUFFER_SIZE - 1] = 0;
	message_from_unix_socket_buffer[MESSAGE_FROM_UNIX_SOCKET_BUFFER_SIZE - 1] = 0;
	unix_socket_len = sizeof(struct sockaddr_un);
	
	/* Init fd_set */
	FD_ZERO(&connections);
	FD_ZERO(&set_tmp);
	
	/* Welcome message */
	printf("Carpantobot 0.1\n\n");
	
	/* Check which program mode we are in */
	if ((retval1 = is_in_client_mode()) == TRUE) {
		program_mode = CLIENT_MODE; 
	} else if (retval1 == EXIT_PROGRAM) {
		exit_entire_program(EXIT_FAILURE);
	} else if((retval1 = is_in_daemon_mode(argc,argv)) == TRUE) {
		program_mode = DAEMON_MODE;
	} else if (retval1 == EXIT_PROGRAM) {
		exit_entire_program(EXIT_FAILURE);
	} else if (is_in_terminal_mode() == TRUE) {
		program_mode = TERMINAL_MODE;
	} else {
		fprintf(stderr,"Can't establish what mode to open program\n");
		exit_entire_program(EXIT_FAILURE);
	}
	
	if(program_mode == TERMINAL_MODE) {
		pid = fork();
		if (pid == -1) {
			fprintf(stderr,"error with fork\n");
			exit_entire_program(EXIT_FAILURE);
		}
	}
	
	
	/* daemon multiplexer */
	if(program_mode == DAEMON_MODE) {
		
		/* multiplexer loop */
		for(;;) {
			set_tmp = connections;
			
			/* Call select and check for error */
#ifdef DEBUG
			printf("Waiting for connection or message from file descriptors\n");
#endif
			retval1 = select(highest_fd + 1,&set_tmp,NULL,NULL,NULL);
#ifdef DEBUG
			printf("Got connection or message\n");
#endif
			if(retval1 == -1) {
				fprintf(stderr,"error with select\n");
				exit_entire_program(EXIT_FAILURE);
			}
			
			/* If we get a connection via Unix socket, check if we're already connected
			 and if not, then connect and add to connections FD_SET */
			if(unix_socket_connected && FD_ISSET(unix_socket,&set_tmp)) {
				if(handle_unix_socket_connection() == EXIT_PROGRAM)
					exit_entire_program(EXIT_FAILURE);
			}
			
			if(current_unix_socket_connected && FD_ISSET(current_unix_socket,&set_tmp)) {
				if((retval1 = handle_message_on_unix_socket()) == EXIT_PROGRAM) 
					exit_entire_program(EXIT_FAILURE);
				if (retval1 == FINISHED_MESSAGE) {
					parse_message_from_unix_socket();
				}
			}
			
		}
	}
	
	/* parent terminal process */
	if(program_mode == TERMINAL_MODE && !pid) {
		/* Adding stdin to connections */
		kill(getpid(),SIGKILL);
	}

	/* child terminal process */
	if(program_mode == TERMINAL_MODE && pid > 0) {
		if(ask_user_for_connection_details(&irc_servers[0]) == EXIT_PROGRAM)
			exit_entire_program(EXIT_SUCCESS);
	}
	
	printf("Exiting...\n");
    return 0;
}
