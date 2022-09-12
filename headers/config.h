#ifndef CONFIG_CUSTOM
#define CONFIG_CUSTOM
#define INPUT_LIMIT 500
#define INPUT_SIZE INPUT_LIMIT + 3
#define QUIT_PROMPT_RESULT_SIZE 4

enum exit_codes {SUCCESS,EXIT_PROGRAM,EOF_SEEN,CONTINUE};

struct irc_server {
	int sock_fd;
	char buffer[255];
	unsigned char buffer_index;
};

struct bot_command_request {
	int sock_fd;
	int server_that_initiated_sock_fd;
	unsigned char request_type;
	unsigned char completed;
	char buffer[255];
	unsigned char buffer_index;
};

extern struct irc_server irc_servers[100];
extern struct bot_command_request bot_command_requests[25];
extern unsigned char irc_servers_index;
extern unsigned char bot_command_requests_index;
extern char quit_prompt_input[QUIT_PROMPT_RESULT_SIZE];
#endif
