#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/config.h"
#include "headers/stdin.h"

signed char ask_user_for_connection_details(void) {
	/* declare local variables */
	signed char retval;
	char port_string[PORT_SIZE];
	char *hostname = irc_servers[0].hostname;
	char *custom_server_name = irc_servers[0].custom_server_name;
	size_t hostname_len = 0;
	
	/* Initialize local variables */
	*port_string = 0;
	port_string[PORT_SIZE - 1] = 0;
	
	/* Fill port_string string */
	do {
		readline_custom("Enter port of IRC server you want to connect to: ('q' to quit) ",port_string,PORT_SIZE);
	} while(port_string_to_short(port_string,&(irc_servers[0].port)) != SUCCESS);
	
	/* Fill irc_servers[0].hostname string */ 
	for(;;) {
		readline_custom("Enter domain name or IP of IRC server you want to connect to ('q' to quit): ",hostname,HOSTNAME_SIZE);
		if((retval = validate_hostname(hostname)) == SUCCESS) {
			break;
		} else if (retval == EXIT_PROGRAM) {
			return EXIT_PROGRAM;
		}
	printf("Not a valid hostname, try again\n");
	}
	
	/* Fill irc_servers[0].custom_server_name */
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
	
	printf("port_string: %s, hostname_string: %s, custom_server_Name_string: %s",port_string,irc_servers[0].hostname,irc_servers[0].custom_server_name);
	
	return SUCCESS;
}
