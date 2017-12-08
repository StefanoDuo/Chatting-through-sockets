#include "command_parser.h"
#include <string.h>
#include <stdio.h>



static const char delimiter[] = ";";



static bool
parse_register_command(char *username, char *ip_address, uint16_t *port_number)
{
    char *token = NULL;

    // parse username
    token = strtok(NULL, delimiter);
    if (token == NULL)
        return false;
    strcpy(username, token);

    // parse ip_address
    token = strtok(NULL, delimiter);
    if (token == NULL)
        return false;
    strcpy(ip_address, token);

    // parse port_number
    token = strtok(NULL, delimiter);
    if (token == NULL)
        return false;
    sscanf(token, "%" SCNu16, port_number);

    // check for trailing stuff
    token = strtok(NULL, delimiter);
    return token == NULL;
}



static bool
parse_deregister_command(void)
{
    // check for trailing stuff
    char *token = strtok(NULL, delimiter);
    return token == NULL;
}



bool
parse_message(char *message, uint16_t *command, char *username, char *ip_address, uint16_t *port_number, char *offline_message)
{
    char *token = NULL;

    token = strtok(message, delimiter);
    if (token == NULL)
        return false;
    // https://stackoverflow.com/questions/12120426/how-to-print-uint32-t-and-uint16-t-variables-value
    // https://stackoverflow.com/questions/33675163/what-is-the-correct-and-portable-clang-gcc-modifier-for-uint16-t-in-sscanf
    // http://en.cppreference.com/w/cpp/types/integer#Format_constants_for_the_std::fscanf_family_of_functions
    sscanf(token, "%" SCNu16, command);

    switch(*command) {
        case REGISTER:
            return parse_register_command(username, ip_address, port_number);
        case DEREGISTER:
            return parse_deregister_command();
        default:
            return false;
    }
}