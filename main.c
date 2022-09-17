#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/config.h"
#include "headers/stdin.h"
struct irc_server irc_servers[MAX_IRC_SERVERS];
struct bot_command_request bot_command_requests[25];
unsigned char irc_servers_index = 0;
unsigned char bot_command_requests_index = 0;
char quit_prompt_input[QUIT_PROMPT_RESULT_SIZE];
char yes_no_prompt[YES_NO_PROMPT_SIZE];

signed char validate_hostname(char *hostname) {
	
	return SUCCESS;
}

signed char port_string_to_short(char *port_string_tmp,unsigned short *port_tmp) {
	
	return SUCCESS;
}

signed char ask_user_for_connection_details() {
	/* declare local variables */
	signed char retval;
	char port_string[PORT_SIZE];
	char *hostname = irc_servers[0].hostname;
	char *custom_server_name = irc_servers[0].custom_server_name;
	size_t hostname_len = 0;
	
	/* Initialize local variables */
	*port_string = 0;
	port_string[PORT_SIZE - 1] = 0;
	
	do {
	readline_custom("Enter port of IRC server you want to connect to: ('q' to quit) ",port_string,PORT_SIZE);
	} while(port_string_to_short(port_string,&(irc_servers[0].port)) != SUCCESS);
	
	for(;;) {
	readline_custom("Enter domain name or IP of IRC server you want to connect to ('q' to quit): ",hostname,HOSTNAME_SIZE);
	if(validate_hostname(hostname) == SUCCESS)
		break;
	printf("Not a valid hostname, try again\n");
	}
	
	for(;;) {
		retval = readline_custom("Would you like to give this server your own name? (y)es/(n)o/(q)uit: ",yes_no_prompt,YES_NO_PROMPT_SIZE);
		if(retval == EXIT_PROGRAM)
			return EXIT_PROGRAM;
		if(!strcmp("No",yes_no_prompt) || !strcmp("no",yes_no_prompt) || !strcmp("N",yes_no_prompt) || !strcmp("n",yes_no_prompt)) {
			hostname_len = strlen(hostname);
			if(hostname_len > CUSTOM_SERVER_NAME_SIZE - 1) {
				fprintf(stderr,"fgets failed to properly terminate the hostname string of the first server for some reason, preventing buffer overflow\n");
				exit(EXIT_FAILURE);
			}
			memcpy(custom_server_name,hostname,hostname_len);
			custom_server_name[hostname_len] = 0;
			break;
		}
		if(strcmp("Yes",yes_no_prompt) && strcmp("yes",yes_no_prompt) && strcmp("Y",yes_no_prompt) && strcmp("y",yes_no_prompt)) {
			printf("Invalid input, please try again\n");
			continue;
		}
		retval = readline_custom("What name would you like to give this server?: ",custom_server_name, CUSTOM_SERVER_NAME_SIZE);
		if(retval == EXIT_PROGRAM)
			return EXIT_PROGRAM;
		break;
	}
	
	return SUCCESS;
}
signed char connect_to_server() {
	
	return SUCCESS;
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	/* Add null bytes to end of each string for safety */
	
	/* Declare local variables */
	int i;
	char *hostname;
	char *custom_server_name;
	
	/* Initialize global variables */
	quit_prompt_input[QUIT_PROMPT_RESULT_SIZE - 1] = 0;
	yes_no_prompt[YES_NO_PROMPT_SIZE - 1] = 0;
	
	/* Initialize structs */
	for(i=0;i < MAX_IRC_SERVERS;i++) {
		hostname = irc_servers[i].hostname;
		*hostname = 0;
		hostname[HOSTNAME_SIZE - 1] = 0;
		custom_server_name = irc_servers[i].custom_server_name;
		*custom_server_name = 0;
		custom_server_name[CUSTOM_SERVER_NAME_SIZE - 1] = 0;
	}
	
	if(ask_user_for_connection_details() == EXIT_PROGRAM)
		exit(EXIT_SUCCESS);
	if(connect_to_server() == EXIT_PROGRAM)
		exit(EXIT_SUCCESS);
	
	
	printf("Exiting...\n");
    return 0;
}
