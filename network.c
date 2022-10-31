#include <stdlib.h>
#include <stdio.h>
#include <sys/select.h>
#include <string.h>
#include <sys/select.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include "headers/config.h"
#include "headers/network.h"
#include "headers/user_interaction.h"

signed char connect_to_server(char *hostname_tmp) {
	/* call getaddrinfo(), prints failure and returns error if we cannot get an IP */
	/* calls success and places IP in IP_tmp if it is a correct host */
	return SUCCESS;
}

signed char ping_current_unix_socket(void) {
	
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

void sort_int_array(int *array_ptr,int array_index) {
	
}

signed char handle_unix_socket_connection(void) {
	int dummy_connection;
	
	if (!current_unix_socket_connected) {
		current_unix_socket = accept(unix_socket,(struct sockaddr *)&unix_socket_address,&unix_socket_len);
		if(current_unix_socket == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
			close(unix_socket);
			unix_socket_connected = 0;
			fprintf(stderr,"fatal error with accept\n");
			return EXIT_PROGRAM;
		} else if (current_unix_socket != -1) {
			FD_SET(current_unix_socket,&connections);
			current_unix_socket_connected = 1;
			return SUCCESS;
		} else {
			printf("error in handle_unix_socket_connection() was: %d\n",errno);
		}/* else connection dropped cleanly after select and before accept, do nothing */
		message_on_unix_socket_buffer_index = 0; /* wipe message buffer just in case */
	} else { /* Accept connection, tell it we're full, then throw it away! */
		dummy_connection = accept(unix_socket,(struct sockaddr *)&unix_socket_address,&unix_socket_len);
		if(dummy_connection != -1) {
			write(dummy_connection,"A client is currently connected, disconnecting...\r\n",34);
			verbose_printf("Discarding extra connection to unix socket after warning them\n");
			close(dummy_connection);
		}
	}
	return SUCCESS;
}

signed char handle_message_on_unix_socket(void) {
	
	/* calculate where in buffer to read into, and how much space is left in buffer */
	char *read_into = message_on_unix_socket_buffer + message_on_unix_socket_buffer_index;
	int rn_present = 0;
	int i = 0;
	int bytes_after_rn = 0;
	
	size_t bytes_to_read = sizeof(message_on_unix_socket_buffer) - message_on_unix_socket_buffer_index;
	
	/* calling recv() */
	ssize_t retval = recv(current_unix_socket,read_into,bytes_to_read,0);
	
	/* check return value of recv() for 0 or less than 0 */
	if(!retval) {
		printf("Unix socket connection client exited properly, closing the connection\n");
		close(current_unix_socket);
		current_unix_socket_connected = 0;
		message_on_unix_socket_buffer_index = 0; /* If a UNIX socket client disconnects, then we discard their 
		* previous messages*/
		FD_CLR(current_unix_socket,&connections);
		return CONNECTION_CLOSED;
	} else if((retval == -1 && errno != EAGAIN && errno != EWOULDBLOCK) || (retval != -1 && retval <= 0)) {
		fprintf(stderr,"Unspecified fatal error reading message from unix socket connection client, exiting program...\n");
		close(current_unix_socket);
		current_unix_socket_connected = 0;
		close(unix_socket);
		unix_socket_connected = 0;
		return EXIT_PROGRAM;
	} else if(retval == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
		fprintf(stderr,"Unix socket connection client dropped connection after it sent a message but before recv(), closing connection\n");
		close(current_unix_socket);
		message_on_unix_socket_buffer_index = 0;
		current_unix_socket_connected = 0;
		FD_CLR(current_unix_socket,&connections);
		return CONNECTION_CLOSED;
	} 
		
	/* recv did not return 0 or -1, bytes_read and retval is greater than 0, parse recv */
	i = message_on_unix_socket_buffer_index;
	
	/* update message_on_unix_socket_buffer_index to position in buffer after newest bytes read */
	message_on_unix_socket_buffer_index = message_on_unix_socket_buffer_index + retval;
	
	/* check bytes read into message_on_unix_socket_buffer for \r\n, and if found put \r\n index in rn_present variable */
	for(;i < message_on_unix_socket_buffer_index;i++) {
		if(message_on_unix_socket_buffer[i] == '\n' && i > 0 && message_on_unix_socket_buffer[i - 1] == '\r') {
			rn_present = i;
			break;
		}
	}
	
	if(rn_present) {
		bytes_after_rn =  message_on_unix_socket_buffer_index - (rn_present + 1); 
		if(rn_present == 1) { /* if \r\n present, but empty message, return NO_MESSAGE */
			if(bytes_after_rn) {
				/* wiping buffer */
				memmove(message_on_unix_socket_buffer,message_on_unix_socket_buffer + 2,bytes_after_rn);
			}
			message_on_unix_socket_buffer_index = 0;
			fprintf(stderr,"empty message sent from Unix socket connection client, ignoring\n");
			return NO_MESSAGE;
		} /* else we have a valid message to parse! */
		
		/* read message_on_unix_socket_buffer into message_from_unix_socket_buffer for parsing after function */
		memcpy(message_from_unix_socket_buffer,message_on_unix_socket_buffer,rn_present); 
		message_from_unix_socket_buffer[rn_present - 1] = 0;
		
		/* */
		
		if(bytes_after_rn) { /* if there were bytes read after \r\n and we didn't fill buffer, wipe */
			memmove(message_on_unix_socket_buffer,message_on_unix_socket_buffer + rn_present + 1,bytes_after_rn);
		}
		message_on_unix_socket_buffer_index = 0;
		
		return FINISHED_MESSAGE;
		
	} else {
		if(retval == bytes_to_read) { /* if hit buffer limit without an \r\n */
			fprintf(stderr,"Unix socket connection client sent too much junk data, no \\r\\n present, closing connection\n");
			close(current_unix_socket);
			current_unix_socket_connected = 0;
			message_on_unix_socket_buffer_index = 0;
			FD_CLR(current_unix_socket,&connections);
			return NO_MESSAGE;
			
		}
		return UNFINISHED_MESSAGE;
	}
}

signed char parse_message_from_unix_socket()
{
				printf("message_from_unix_socket_buffer: %s\n",message_from_unix_socket_buffer);
	
}
