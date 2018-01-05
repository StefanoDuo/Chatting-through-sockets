#ifndef CLIENT_COMMAND_PARSER_H
#define CLIENT_COMMAND_PARSER_H



#include <inttypes.h>
#include <stdbool.h>



/* Used to parse a generic command
 *
 * Returns false if the command is unknown or its format is wrong
 */
bool
parse_command(char *str, int16_t *command, char *username);



/* Used to compose a multi-line message during a !send
 *
 * Returns false if the message exceeded max_size
 */
bool
parse_message(char *str, uint16_t max_size);



#endif
