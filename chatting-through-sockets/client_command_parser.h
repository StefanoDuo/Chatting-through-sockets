#ifndef CLIENT_COMMAND_PARSER_H
#define CLIENT_COMMAND_PARSER_H



#include <inttypes.h>
#include <stdbool.h>



bool
parse_command(char * str, int16_t *command, char *username);



// TODO: recv server response in every command



#endif
