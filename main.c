#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/config.h"
#include "headers/user_interaction.h"
#include "headers/network.h"
struct irc_server irc_servers[MAX_IRC_SERVERS];
struct bot_command_request bot_command_requests[25];
unsigned char irc_servers_index = 0;
unsigned char bot_command_requests_index = 0;
char quit_prompt_input[QUIT_PROMPT_RESULT_SIZE];
char yes_no_prompt[YES_NO_PROMPT_SIZE];

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
