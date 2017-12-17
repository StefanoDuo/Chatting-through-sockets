#ifndef CLIENT_COMMAND_PARSER_H
#define CLIENT_COMMAND_PARSER_H



#include <inttypes.h>
#include <stdbool.h>



bool
parse_command(char *str, int16_t *command, char *username);



bool
parse_message(char *str, uint16_t max_size);



#endif
