#ifndef NETWORK_H
#define NETWORK_H

signed char connect_to_server(char *hostname_tmp);
signed char port_string_to_short(char *port_string_tmp,unsigned short *port_tmp);
signed char ping_current_unix_socket(void);
signed char handle_unix_socket_connection(void);
signed char handle_message_on_unix_socket(void);
signed char parse_message_from_unix_socket(void);

#endif
