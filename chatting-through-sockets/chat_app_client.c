#include "chat_app_client.h"
#include "client_command_parser.h"
#include "safe_tcp.h"
#include "safe_socket.h"
#include "chat_app_sizes.h"

#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>



void
execute_register(int server_conn_sd, const char *username)
{
    char message[MAX_MESSAGE_LENGTH];
    ssprintf(message, "%" PRId16 ";%s", REGISTER, username);
    printf(message);
    tcp_send(server_conn_sd, message);
}



void
execute_who(int server_conn_sd)
{
    char message[MAX_MESSAGE_LENGTH];
    ssprintf(message, "%" PRId16, WHO);
    printf(message);
    tcp_send(server_conn_sd, message);
}



void
execute_quit(int server_conn_sd)
{
    char message[MAX_MESSAGE_LENGTH];
    ssprintf(message, "%" PRId16, QUIT);
    printf(message);
    tcp_send(server_conn_sd, message);
}



void
execute_deregister(int server_conn_sd)
{
    char message[MAX_MESSAGE_LENGTH];
    ssprintf(message, "%" PRId16, DEREGISTER);
    printf(message);
    tcp_send(server_conn_sd, message);
}



void
execute_send(int server_conn_sd, const char *username)
{
   //TODO: implement
}



void
execute_command(int server_conn_sd, const char* str)
{
    char username[MAX_USERNAME_LENGTH];
    int16_t command;

    bool success = parse_command(&command, username);
    if (!success) {
        printf("Unknown command\n");
        return;
    }

    switch(command) {
        case REGISTER:
            execute_register(server_conn_sd, username);
            break;
        case WHO:
            execute_who(server_conn_sd);
            break;
        case QUIT:
            execute_quit(server_conn_sd);
            break;
        case DEREGISTER:
            execute_deregister(server_conn_sd);
            break;
        case SEND:
            execute_send(server_conn_sd, username);
            break;
    }
}