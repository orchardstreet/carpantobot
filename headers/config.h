#ifndef CONFIG_CUSTOM
#define CONFIG_CUSTOM
#include <sys/select.h>
#include <sys/socket.h>
#define DEBUG
#define UNIX_SOCKET_PATH "/tmp/carpantobot_unix.sock"
#define UNIX_SOCKET_LOCK_PATH "/tmp/carpantobot_unix.lock"
#define INPUT_LIMIT 500
#define HOSTNAME_LIMIT 50
#define INPUT_SIZE INPUT_LIMIT + 3
#define HOSTNAME_SIZE HOSTNAME_LIMIT + 3
#define CUSTOM_SERVER_NAME_SIZE HOSTNAME_SIZE 
#define QUIT_PROMPT_RESULT_SIZE 4
#define YES_NO_PROMPT_SIZE 6
#define PORT_SIZE 8
#define MESSAGE_ON_UNIX_SOCKET_BUFFER_SIZE 10
#define MESSAGE_FROM_UNIX_SOCKET_BUFFER_SIZE MESSAGE_ON_UNIX_SOCKET_BUFFER_SIZE + 1 
#define MAX_IRC_SERVERS 100
#define MAX_FDS MAX_IRC_SERVERS + 3

enum exit_codes {SUCCESS,EXIT_PROGRAM,EOF_SEEN,CONTINUE,FAILURE,TRUE,FALSE, UNFINISHED_MESSAGE, FINISHED_MESSAGE, CONNECTION_CLOSED, NO_MESSAGE};
enum modes {TERMINAL_MODE,DAEMON_MODE,CLIENT_MODE};
enum output_modes {NORMAL,VERBOSE,O_DEBUG};

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
extern fd_set connections, set_tmp;
extern unsigned char daemon_mode;
extern int unix_socket;
extern struct sockaddr_un unix_socket_address;
extern unsigned char current_unix_socket_connected;
extern unsigned char unix_socket_connected;
extern int current_unix_socket;
extern int highest_fd;
extern socklen_t unix_socket_len;
extern char message_on_unix_socket_buffer[MESSAGE_ON_UNIX_SOCKET_BUFFER_SIZE];
extern int message_on_unix_socket_buffer_index;
extern char message_from_unix_socket_buffer[MESSAGE_FROM_UNIX_SOCKET_BUFFER_SIZE];
extern unsigned char log_mode;
#endif
