#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/stdin.h"
#include "headers/config.h"

signed char flush_stdin(void) {
	int charr;
	for(;;) {
		charr = getc(stdin);
		if(charr == '\n') {
			return CONTINUE;
		} else if (charr == EOF) {
			return EOF_SEEN;
		}
	}
}

signed char quit_prompt(void) {
	/* Asks user if he wants to quit or not, adds answer to quit_prompt_input variable 
	 * Attempts to read from stdin to memory, until a valid input is given.
	 * Asks user again on no valid input, no input, or on overflow
	 * This program also flushes stdin on overflow.
	 * This function deals with pecularities of stdin redirect input, in case
	 * someone is automating the program with stdin redirect.
	 * RETURN VALUES: uses the exit_codes enum, which is self-explanatory */
	
	size_t fgets_input_length = 0;
	signed char retval;
	
	for(;;) {
		
		printf("Would you really like to quit? (y/n): \n");
		
		if (fgets(quit_prompt_input,QUIT_PROMPT_RESULT_SIZE - 1,stdin) == NULL) {
			if(ferror(stdin)) {
				perror("");
				fprintf(stderr,"fgets failure\n");
				return EXIT_PROGRAM; 
			} else if (feof(stdin)) {
				printf("detected EOF\n");
				clearerr(stdin);
				return EXIT_PROGRAM;
			} else {
				fprintf(stderr,"unhandled error\n");
				return EXIT_PROGRAM; /* exit */
			}
		}
		
		/* CHECK FOR OVERFLOW, this should be the first check in this function that doesn't exit entire program (besides EOF check) */
		/* if what user enters reaches (input_size_temp - 3) without newline, count as overflow, flush stdin and ask again */
		fgets_input_length = strlen(quit_prompt_input);
		if((fgets_input_length == QUIT_PROMPT_RESULT_SIZE - 2) && (quit_prompt_input[QUIT_PROMPT_RESULT_SIZE - 3] != '\n')) {
			/* flush stdin */
			printf("Overflow detected, cannot enter more than %d characters\n",QUIT_PROMPT_RESULT_SIZE - 3);
			retval = flush_stdin();
			if (retval == CONTINUE) {
				continue;
			} else if (retval == EOF_SEEN) {
				clearerr(stdin);
				return EXIT_PROGRAM;
			}
		}
		
		/* We didn't overflow stdin */
		
		/* If nothing entered, jump to top of function */
		if(quit_prompt_input[0] == '\n' || *quit_prompt_input == 0) {
			fprintf(stderr,"Nothing entered, try again");
			continue;
		}
		
		/* remove newline if there is one */
		/* we already made sure fgets_input_length is at least 1 */
		if(quit_prompt_input[fgets_input_length - 1] == '\n') {
			quit_prompt_input[fgets_input_length - 1] = 0;
		} else {
			printf("\n");
		}
		
		if (*quit_prompt_input == 'y' || *quit_prompt_input == 'Y') {
			return EXIT_PROGRAM;
		} else if (*quit_prompt_input == 'n' || *quit_prompt_input == 'N') {
			return CONTINUE;
		} else {
			printf("Invalid input\n");
			continue;
		}
		
	}
	
	return SUCCESS; /* success */
} 

signed char readline_custom(char *prompt, char *input, size_t input_size_temp) {
	/* !! Don't pass a pointer to an array of size less than 4 !! */
	/* Asks user for input.
	 * Attempts to read from stdin to memory, until a valid input is given.
	 * Asks user again on no input, on overflow, or if detects a special
	 * character.
	 * This program also flushes stdin on overflow.
	 * This function deals with pecularities of stdin redirect input, in case
	 * someone is automating the program with stdin redirect.
	 * RETURN VALUES: uses the exit_codes enum, which is self-explanatory */
	
	size_t fgets_input_length = 0;
	char *end_of_input;
	char *browse_input;
	unsigned char special_character_present = 0;
	signed char retval;
	
	for(;;) {
		
		printf("%s\n",prompt);
		
		if (input == NULL) {
			fprintf(stderr,"passed NULL pointer to get_server_info_from_stdin() function\n");
			return EXIT_PROGRAM; /* exit */
		}
		
		if (input_size_temp < 4) {
			fprintf(stderr,"buffer provided not enough to read into fgets in function get_server_info_from_stdin\n");
			return EXIT_PROGRAM; /* exit */
		}
		
		if (fgets(input,input_size_temp - 1,stdin) == NULL) {
			if(ferror(stdin)) {
				perror("");
				fprintf(stderr,"fgets failure\n");
				return EXIT_PROGRAM; 
			} else if (feof(stdin)) {
				printf("detected EOF\n");
				clearerr(stdin);
				if(quit_prompt() == EXIT_PROGRAM)
					return EXIT_PROGRAM;
				continue;
			} else {
				fprintf(stderr,"unhandled error\n");
				return EXIT_PROGRAM; /* exit */
			}
		}
		
		/* CHECK FOR OVERFLOW, this should be the first check in this function that doesn't exit entire program (besides EOF check) */
		/* if what user enters reaches (input_size_temp - 3) without newline, count as overflow, flush stdin and ask again */
		fgets_input_length = strlen(input);
		if((fgets_input_length == input_size_temp - 2) && (input[input_size_temp - 3] != '\n')) {
			/* flush stdin */
			printf("Overflow detected, cannot enter more than %d characters\n",QUIT_PROMPT_RESULT_SIZE - 3);
			retval = flush_stdin();
			if (retval == CONTINUE) {
				continue;
			} else if (retval == EOF_SEEN) {
				clearerr(stdin);
				if(quit_prompt() == EXIT_PROGRAM)
					return EXIT_PROGRAM;
				continue;
			}
		}
		
		/* We didn't overflow stdin */
		
		/* If nothing entered, jump to top of function */
		if(input[0] == '\n' || *input == 0) {
			fprintf(stderr,"Nothing entered, try again");
			continue;
		}
		
		/* remove newline if there is one */
		/* we already made sure fgets_input_length is at least 1 */
		if(input[fgets_input_length - 1] == '\n') {
			input[fgets_input_length - 1] = 0;
		} else {
			printf("\n");
		}
		
		if(*input == 'q' && input[1] == 0) {
			if(quit_prompt() == EXIT_PROGRAM)
				return EXIT_PROGRAM;
			continue;
		}
		
		/* Check if special character exists in input, 
		 * up to null character */
		for(end_of_input = input + fgets_input_length - 1,browse_input = input;browse_input <= end_of_input;browse_input++) {
			if(*browse_input < ' ' || *browse_input > '~') {
				special_character_present = 1;
				break;
			}
		}
		
		if(special_character_present) {
			fprintf(stderr,"no special characters in input please\n");
			special_character_present = 0;
			continue;
		} else {
			break; /* leave entire function as SUCCESS */
		}
	}
	
	return SUCCESS; /* success */
} 
