#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H



#include <stdbool.h>
#include <inttypes.h>



#define REGISTER 0
#define WHO 1
#define QUIT 2
#define DEREGISTER 3
#define SEND 4



bool
parse_message(char *message, uint16_t *command, char *username, char *ip_address, uint16_t *port_number, char *offline_message);



#endif