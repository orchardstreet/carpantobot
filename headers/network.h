#ifndef NETWORK_HEADER
#define NETWORK_HEADER

signed char validate_hostname(char *hostname); 
signed char port_string_to_short(char *port_string_tmp,unsigned short *port_tmp) ;
signed char connect_to_server();


#endif
