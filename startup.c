#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "headers/startup.h"
#include "headers/config.h"
#include "headers/user_interaction.h"

signed char is_in_args(int number_of_args,char **args,char *argument) {
	
	int i;
	
	if(number_of_args < 2)
		return FAILURE;
	
	for(i = 1;i < number_of_args; i++) {
		if(!strcmp(argument,args[i]))
			return SUCCESS;
	}
	
	return FAILURE;
	
}

signed char is_in_client_mode(void) {
	/* create a unix socket, try to connect to the socket, send a message to see if awake, if not awake then exit entire program */
	verbose_printf("Testing CLIENT_MODE\n");
	verbose_printf("Creating UNIX socket...\n");
	if((unix_socket = socket(AF_UNIX,SOCK_STREAM,0)) == -1) {
		fprintf(stderr,"Error opening socket, exiting...\n");
		return EXIT_PROGRAM;
	}
	verbose_printf("UNIX socket successfully created\n");
	unix_socket_connected = 1;
	
	memset(&unix_socket_address,0,sizeof(unix_socket_address));
	unix_socket_address.sun_family = AF_UNIX;
	
	if(sizeof(UNIX_SOCKET_PATH) > sizeof(unix_socket_address.sun_path)) {
		fprintf(stderr,"Length of UNIX_SOCKET_PATH: %zu exceeds the length of sun_path: %zu bytes."
		"Cannot store UNIX_SOCKET_PATH, exiting...\n",sizeof(UNIX_SOCKET_PATH),sizeof(unix_socket_address.sun_path));
		close(unix_socket);
		unix_socket_connected = 0;
		return EXIT_PROGRAM;
	}
	
	memcpy(unix_socket_address.sun_path,UNIX_SOCKET_PATH,sizeof(UNIX_SOCKET_PATH)); /* path is /tmp/carpantobot.sock by default */
	if(connect(unix_socket,(struct sockaddr *)&unix_socket_address,sizeof(struct sockaddr_un)) == -1) {
		verbose_printf("Connect() cannot find a listening carpantobot daemon: %d\n",errno);
		if(errno == EAFNOSUPPORT || errno == EACCES || errno == EADDRINUSE || errno == EADDRNOTAVAIL || errno == EBADF || errno == EFAULT || errno == EINTR || errno == EISCONN || errno == ENETUNREACH || errno == ENOTSOCK || errno == EPROTOTYPE) {
			fprintf(stderr,"Fatal error while trying to connect to Unix socket to determine if in client mode\n");
			close(unix_socket);
			unix_socket_connected = 0;
			return EXIT_PROGRAM;
		} else {
			verbose_printf("Couldn't launch in CLIENT_MODE\nGoing to try DAEMON_MODE\n");
			return FALSE;
		}
	}
	
	verbose_printf("We are in CLIENT_MODE\n");
	return TRUE;
}


signed char is_in_daemon_mode(int number_of_args, char **args) {
	int lock_fd;
	int retval;

verbose_printf("Testing DAEMON_MODE\n");
if (is_in_args(number_of_args,args,"-d") == FAILURE) {
	verbose_printf("DAEMON_MODE not specified in program startup (to do so use \"-d\" as a startup option)\n");
	verbose_printf("Closing UNIX socket\n");
	close(unix_socket);
	unix_socket_connected = 0;
	verbose_printf("Going to try TERMINAL_MODE\n");
	return FALSE;
}

lock_fd = open(UNIX_SOCKET_LOCK_PATH,O_RDONLY|O_CREAT,0600);
if(lock_fd == -1) {
	fprintf(stderr,"Couldn't create or open lock file for unix socket connection daemon\n");
	close(unix_socket);
	unix_socket_connected = 0;
	return EXIT_PROGRAM;
}
retval = flock(lock_fd,LOCK_EX|LOCK_NB);
if(retval) {
	fprintf(stderr,"%s is locked.  This means it is in use by another person," 
	"Cannot launch daemon mode, exiting...\n",UNIX_SOCKET_LOCK_PATH);
	close(unix_socket);
	unix_socket_connected = 0;
	return EXIT_PROGRAM;
}

unlink(UNIX_SOCKET_PATH);

/* Check lock file and then unlink previous UNIX socket file, should it exist */
verbose_printf("Binding unix socket for DAEMON MODE server\n");
if(bind(unix_socket,(struct sockaddr *)&unix_socket_address,sizeof(struct sockaddr_un)) == -1) {
	fprintf(stderr,"Couldn't bind to socket in daemon mode, exiting program...\n");
	close(unix_socket);
	unix_socket_connected = 0;
	return EXIT_PROGRAM;
}

unix_socket_connected = 1;

verbose_printf("Listening to unix socket for connections\n");
if(listen(unix_socket,3) == -1) {
	close(unix_socket);
	unix_socket_connected = 0;
	fprintf(stderr,"Error while trying to listen to unix socket while in daemon mode\n");
	return EXIT_PROGRAM;
}

FD_SET(unix_socket,&connections);

verbose_printf("We are in DAEMON_MODE\n");
return TRUE;
}

signed char is_in_terminal_mode(void) {
	verbose_printf("Trying to launch in TERMINAL_MODE\n");
	if(isatty(0) && isatty(1)) {
		verbose_printf("We are in TERMINAL_MODE\n");
		return TRUE;
	}
	verbose_printf("Couldn't open program in TERMINAL_MODE, not using a proper terminal\n");
	return FALSE;
}
