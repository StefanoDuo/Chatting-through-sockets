#include "client_command_parser.h"
#include "commands_format.h"

#include <string.h>
#include <stdio.h>



#define CLIENT_DELIMITER " "



static bool
check_trailing_stuff(void)
{
    char *token = strtok(NULL, CLIENT_DELIMITER);
    return token == NULL;
}



static bool
parse_register(char *username)
{
    char *token = strtok(NULL, CLIENT_DELIMITER);
    if (token == NULL)
        return false;
    strcpy(username, token);

    return check_trailing_stuff();
}



static bool
parse_who(void)
{
    return check_trailing_stuff();
}



static bool
parse_quit(void)
{
    return check_trailing_stuff();
}



static bool
parse_deregister(void)
{
    return check_trailing_stuff();
}



static bool
parse_send(char *username)
{
    char *token = strtok(NULL, CLIENT_DELIMITER);
    if (token == NULL)
        return false;
    strcpy(username, token);

    return check_trailing_stuff();
}



static bool
parse_help(void)
{
    return check_trailing_stuff();
}



static int16_t
match_command(const char *command)
{
    if (strcmp(command, "!register") == 0)
        return REGISTER;
    if (strcmp(command, "!who") == 0)
        return WHO;
    if (strcmp(command, "!quit") == 0)
        return QUIT;
    if (strcmp(command, "!deregister") == 0)
        return DEREGISTER;
    if (strcmp(command, "!send") == 0)
        return SEND;
    if (strcmp(command, "!help") == 0)
        return HELP;
    return UNKNOWN_COMMAND;
}



bool
parse_command(char *str, int16_t *command, char *username)
{
    char *token = NULL;

    // parse command
    token = strtok(str, CLIENT_DELIMITER);
    if (token == NULL)
        return false;

    *command = match_command(token);
    switch(*command) {
        case REGISTER:
            return parse_register(username);
        case WHO:
            return parse_who();
        case QUIT:
            return parse_quit();
        case DEREGISTER:
            return parse_deregister();
        case SEND:
            return parse_send(username);
        case HELP:
            return parse_help();
    }
    return false;
}



/* Used to compose a multi-line message during a !send
 *
 * Returns false if the message exceeded max_size
 */
bool
parse_message(char *str, uint16_t max_size)
{
	uint16_t current_size = 0;	
	do {
		/* fgets reads max_size - current_size - 1 characters at most
		 * therefore when we only have 1 byte of space left in str
		 * it doesn't read anything, to handle that case we exit from the while
		 * and read into an auxiliary buffer to check if the user wrote "./n"
		 */
		fgets(str, max_size - current_size, stdin);
		if (strcmp(str, ".\n") == 0) {
			*str = '\0';
			return true;
		}
		current_size += strlen(str);
		str += strlen(str);
	} while (current_size < max_size - 1);
	
	if (max_size - current_size == 1) {
		/* We have read a message of max_size, now we need to check if the user
		 * wrote "./n" to end the message
		 */
		char buffer[3];
		fgets(buffer, sizeof(buffer), stdin);
		if (strcmp(buffer, ".\n") == 0) {
			*str = '\0';
			return true;
		}
	}
	
	return false;
}
















