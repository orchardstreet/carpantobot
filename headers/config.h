#ifndef CONFIG_CUSTOM
#define CONFIG_CUSTOM
#define INPUT_LIMIT 500
#define HOSTNAME_LIMIT 50
#define INPUT_SIZE INPUT_LIMIT + 3
#define HOSTNAME_SIZE HOSTNAME_LIMIT + 3
#define CUSTOM_SERVER_NAME_SIZE HOSTNAME_SIZE 
#define QUIT_PROMPT_RESULT_SIZE 4
#define YES_NO_PROMPT_SIZE 6
#define PORT_SIZE 8
#define MAX_IRC_SERVERS 100

enum exit_codes {SUCCESS,EXIT_PROGRAM,EOF_SEEN,CONTINUE,FAILURE};

struct irc_server {
	int sock_fd;
	char hostname[HOSTNAME_SIZE]; char custom_server_name[CUSTOM_SERVER_NAME_SIZE];
	unsigned short port;
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

extern struct irc_server irc_servers[MAX_IRC_SERVERS];
extern struct bot_command_request bot_command_requests[25];
extern unsigned char irc_servers_index;
extern unsigned char bot_command_requests_index;
extern char quit_prompt_input[QUIT_PROMPT_RESULT_SIZE];
extern char yes_no_prompt[YES_NO_PROMPT_SIZE];
#endif
