#include "chat_app_client.h"
#include "client_command_parser.h"
#include "safe_tcp.h"
#include "safe_udp.h"
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
initialize_chat_client(const char *ip, uint16_t port)
{
	strcpy(local_ip, ip);
	local_port = port;
}



static void
check_for_offline_messages(int server_conn_sd)
{
	char *buffer = malloc(MAX_BUFFER_SIZE);
	if (buffer == NULL) {
		printf("Error during malloc(): out of memory\n");
		exit(-1);
	}
	
	tcp_receive(server_conn_sd, buffer, MAX_BUFFER_SIZE);
    int16_t how_many;
    sscanf(buffer, "%" SCNd16, &how_many);
    
    printf("You received %" PRId16 " messages while offline\n", how_many);
    for (; how_many > 0; --how_many) {
    	tcp_receive(server_conn_sd, buffer, MAX_BUFFER_SIZE);
    	printf("%s\n", buffer);
    }
    
    free(buffer);
}



static void
execute_register(int server_conn_sd, const char *username)
{
    char message[MAX_MESSAGE_LENGTH];
    
    if (is_username_set) {
    	printf("You are alredy registered\n");
    	return;
    }
    
    if (strchr(username, C_DELIMITER)) {
    	printf("Your username cannot contain '%s'\n", DELIMITER);
    	return;
    }
    
    sprintf(message, "%" PRId16 ";%s;%s;%" PRIu16, REGISTER, username, local_ip, local_port);
    tcp_send(server_conn_sd, message);
    
    int16_t result;
    char *token = NULL;
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
    
    check_for_offline_messages(server_conn_sd);
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
    
    strcpy(local_username, "");
    is_username_set = false;
    printf("Successfully deregistered\n");
}


static void
send_offline(int server_conn_sd, const char *dest_username, const char *message)
{
	char *buffer = malloc(MAX_BUFFER_SIZE);
	if (buffer == NULL) {
		printf("Error during malloc(): out of memory\n");
		exit(-1);
	}
	
	sprintf(buffer,
			"%" PRId16 ";%s;%s>%s",
			SEND,
			dest_username,
			local_username,
			message);
	tcp_send(server_conn_sd, buffer);
	tcp_receive(server_conn_sd, buffer, MAX_BUFFER_SIZE);
	printf("%s\n", buffer);
	
	free(buffer);
}



static void
execute_send(int server_conn_sd, int udp_sd, const char *username)
{
	if (!is_username_set) {
		printf("You must be registered to send a message\n");
		return;
	}
	
    char message[MAX_MESSAGE_LENGTH];
	
	// first we need to translate username into a pair (ip_address, port_number)
	sprintf(message, "%" PRId16 "%s%s", RESOLVE_NAME, DELIMITER, username);
	tcp_send(server_conn_sd, message);
	tcp_receive(server_conn_sd, message, sizeof(message));
	printf("Received: %s\n", message);
	
	char ip_address[INET_ADDRSTRLEN];
	uint16_t port_number;
	int16_t result_code;
	char *token = strtok(message, DELIMITER);
	sscanf(token, "%" SCNd16, &result_code);
	
	if (result_code == USERNAME_NOT_FOUND) {
		printf("%s is not a registered username\n", username);
		return;
	}
	
	if (result_code == USERNAME_ONLINE) {
		token = strtok(NULL, DELIMITER);
		strcpy(ip_address, token);
		token = strtok(NULL, DELIMITER);
		sscanf(token, "%" SCNu16, &port_number);
	}
	
	// parse message
	bool parsing_completed = parse_message(message, sizeof(message));
	if (!parsing_completed) {
		printf("Message too log. Max message size: %" PRId16 "\n", MAX_MESSAGE_LENGTH);
		return;
	}
	
	if (result_code == USERNAME_NOT_ONLINE) {
		send_offline(server_conn_sd, username, message);
		return;
	}
	
	// result_code == USERNAME_ONLINE
	char *buffer = malloc(MAX_BUFFER_SIZE);
    if (buffer == NULL) {
    	printf("Error during malloc(): out of memory\n");
    	exit(-1);
    }
    
	sprintf(buffer, "%s>%s", local_username, message);
	udp_send(udp_sd, buffer, ip_address, port_number);

	free(buffer);
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
execute_command(int server_conn_sd, int udp_sd, char* str)
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
            execute_send(server_conn_sd, udp_sd, username);
            break;
        case HELP:
            execute_help();
            break;
    }
}
