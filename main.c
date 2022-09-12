#include <stdio.h>
#include <stdlib.h>
#include "headers/config.h"
#include "headers/stdin.h"
struct irc_server irc_servers[100];
struct bot_command_request bot_command_requests[25];
unsigned char irc_servers_index = 0;
unsigned char bot_command_requests_index = 0;
char quit_prompt_input[QUIT_PROMPT_RESULT_SIZE];

void ask_user_for_connection_details() {
	
}
void connect_to_server() {
	
}

int main(int argc, char *argv[])
{
	/* Initialize variables */
	(void)argc;
	(void)argv;
	quit_prompt_input[QUIT_PROMPT_RESULT_SIZE - 1] = 0;
	
	ask_user_for_connection_details();
	connect_to_server();
	
	
    return 0;
}
