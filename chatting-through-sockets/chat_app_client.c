#include "chat_app_client.h"
#include "client_command_parser.h"
#include "safe_tcp.h"
#include "safe_socket.h"
#include "chat_app_sizes.h"
#include "commands_format.h"

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>



static char local_ip[INET_ADDRSTRLEN];
static uint16_t local_port;
static bool is_username_set = false;
char local_username[MAX_USERNAME_LENGTH] = "";



void
set_ip_and_port(const char *ip, uint16_t port)
{
	strcpy(local_ip, ip);
	local_port = port;
}



static void
execute_register(int server_conn_sd, const char *username)
{
    char message[MAX_MESSAGE_LENGTH];
    if (is_username_set) {
    	printf("You are alredy registred\n");
    	return;
    }
    sprintf(message, "%" PRId16 ";%s;%s;%" PRIu16, REGISTER, username, local_ip, local_port);
    tcp_send(server_conn_sd, message);
    
    int16_t result;
    char *token;
    tcp_receive(server_conn_sd, message, sizeof(message));
   	token = strtok(message, DELIMITER);
    sscanf(token, "%" SCNd16, &result);
    token = strtok(NULL, DELIMITER);
    if (result != SUCCESS) {
    	printf("%s\n", token);
    } else {
    	printf("Successfully registered\n");
    	strcpy(local_username, username);
    	is_username_set = true;
    }
}



static void
execute_who(int server_conn_sd)
{
    char message[MAX_MESSAGE_LENGTH];
    sprintf(message, "%" PRId16, WHO);
    tcp_send(server_conn_sd, message);
    
    tcp_receive(server_conn_sd, message, sizeof(message));
    int16_t how_many;
    sscanf(message, "%" SCNd16, &how_many);
    
    printf("Registered clients:\n");
    for (; how_many > 0; --how_many) {
    	tcp_receive(server_conn_sd, message, sizeof(message));
    	printf("\t%s\n", message);
    }
}



static void
execute_quit(int server_conn_sd)
{
    char message[MAX_MESSAGE_LENGTH];
    sprintf(message, "%" PRId16, QUIT);
    tcp_send(server_conn_sd, message);
    exit(0);
}



static void
execute_deregister(int server_conn_sd)
{
	if (!is_username_set) {
		printf("You are not registerd\n");
		return;
	}
    char message[MAX_MESSAGE_LENGTH];
    sprintf(message, "%" PRId16, DEREGISTER);
    tcp_send(server_conn_sd, message);
    //tcp_receive(server_conn_sd, message);
    
    strcpy(local_username, "");
    is_username_set = false;
    printf("Successfully deregistered\n");
}



static void
execute_send(int server_conn_sd, const char *username)
{
   //TODO: implement
}



void
execute_help(void)
{
   printf("Available commands:\n");
   printf("!help --> shows available commands\n");
   printf("!register <username> --> register client as <username>\n");
   printf("!deregister --> deregister client\n");
   printf("!who --> shows registered clients\n");
   printf("!send <username> --> sends a message to another registered client\n");
   printf("!quit --> sets the username offline and exits\n");
}



void
execute_command(int server_conn_sd, char* str)
{
    char username[MAX_USERNAME_LENGTH];
    int16_t command;

    bool success = parse_command(str, &command, username);
    if (!success) {
    	if (command == UNKNOWN_COMMAND)
        	printf("Unknown command\n");
        else
        	printf("Wrong command format\n");
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
        case HELP:
            execute_help();
            break;
    }
}
