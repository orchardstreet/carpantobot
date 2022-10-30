#ifndef STARTUP_H
#define STARTUP_H

signed char is_in_daemon_mode(int number_of_args, char **args); 

signed char is_in_client_mode(void);

signed char is_in_terminal_mode(void);

#endif
