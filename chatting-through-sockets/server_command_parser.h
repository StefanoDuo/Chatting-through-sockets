#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H



#include <stdbool.h>
#include <inttypes.h>



bool
parse_message(char *message, int16_t *command, char *username, char *ip_address,
			  uint16_t *port_number, char *offline_message);



#endif
