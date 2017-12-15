#include "command_parser.h"
#include "commands_format.h"

#include <string.h>
#include <stdio.h>



static const char delimiter[] = ";";



static bool
check_trailing_stuff(void)
{
    char *token = strtok(NULL, delimiter);
    return token == NULL;
}



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

    return check_trailing_stuff();
}

static bool
parse_deregister_command(void)
{
    return check_trailing_stuff();
}



static bool
parse_who_command(void)
{
    return check_trailing_stuff();
}



static bool
parse_quit_command(void)
{
    return check_trailing_stuff();
}



static bool
parse_resolve_name_command(char * username)
{
    char *token = NULL;

    // parse username
    token = strtok(NULL, delimiter);
    if (token == NULL)
        return false;
    strcpy(username, token);

    return check_trailing_stuff();
}



bool
parse_message(char *message, int16_t *command, char *username, char *ip_address, uint16_t *port_number, char *offline_message)
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
        case WHO:
            return parse_who_command();
        case QUIT:
            return parse_quit_command();
        case DEREGISTER:
            return parse_deregister_command();
        case RESOLVE_NAME:
            return parse_resolve_name_command(username);
    }
}