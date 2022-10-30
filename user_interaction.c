#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "headers/config.h"
#include "headers/stdin.h"
#include "headers/network.h"

void verbose_printf(char* message,...)
{
	if(log_mode == VERBOSE) {
		va_list args;
		va_start(args,message);
		vprintf(message,args);
		va_end(args);
	}
}

signed char ask_user_for_connection_details(struct irc_server *server_tmp) {
	/* declare local variables */
	signed char retval;
	char port_string[PORT_SIZE];
	char *hostname = server_tmp->hostname;
	char *custom_server_name = server_tmp->custom_server_name;
	size_t hostname_len = 0;
	
	/* Initialize local variables */
	*port_string = 0;
	port_string[PORT_SIZE - 1] = 0;
	
	/* Fill server_tmp->port */
	do {
		retval = readline_custom("Enter port of IRC server you want to connect to: (or 'q' to quit): ",port_string,PORT_SIZE);
		if(retval == EXIT_PROGRAM)
			return EXIT_PROGRAM;
	} while(port_string_to_short(port_string,&(server_tmp->port)) != SUCCESS);
	
	/* Fill server_tmp->hostname string */ 
	retval = readline_custom("Enter hostname of server you want to connect to (or 'q' to quit): ",hostname,HOSTNAME_SIZE);
	if(retval == EXIT_PROGRAM)
		return EXIT_PROGRAM;
	
	/* Fill server_tmp->custom_server_name */
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
			memcpy(custom_server_name,hostname,hostname_len + 1);
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
	
	
	printf("You entered:\nport: %u\nhostname: %s\ncustom name: %s\n",server_tmp->port,server_tmp->hostname,server_tmp->custom_server_name);
	
	return SUCCESS;
}
