#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "headers/config.h"

signed char connect_to_server(char *hostname_tmp) {
	/* call getaddrinfo(), prints failure and returns error if we cannot get an IP */
	/* calls success and places IP in IP_tmp if it is a correct host */
	return SUCCESS;
}

signed char port_string_to_short(char *port_string_tmp,unsigned short *port_tmp) {
	long result;
	char *endptr;
	result = strtol(port_string_tmp,&endptr,10);
	if(result < 1 || result > 65535 || *endptr != '\0') {
		printf("Invalid port number, must be between 1-65535 inclusive, try again\n");
		return FAILURE;
	}
	/* port read successful */
	*port_tmp = (unsigned short)result;
	return SUCCESS;
}

