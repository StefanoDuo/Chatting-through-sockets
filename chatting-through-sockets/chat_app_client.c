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
    	printf("%s", buffer);
    }
    
    free(buffer);
}



static void
execute_register(int server_conn_sd, const char *username)
{
    if (is_username_set) {
    	printf("You are alredy registered\n");
    	return;
    }
    if (strchr(username, C_DELIMITER)) {
    	printf("Your username cannot contain '%s'\n", DELIMITER);
    	return;
    }
    if (strlen(username)+1 > MAX_USERNAME_LENGTH) {
    	printf("Your username cannot be longer than %" PRIu16 "\n", MAX_USERNAME_LENGTH-1);
    	return;
    }
    
    char *message = malloc(MAX_BUFFER_SIZE);
	if (message == NULL) {
		printf("Error during malloc(): out of memory\n");
		exit(-1);
	}
    
    sprintf(message, "%" PRId16 ";%s;%s;%" PRIu16, REGISTER, username, local_ip, local_port);
    tcp_send(server_conn_sd, message);
    
    int16_t result;
    char *token = NULL;
    tcp_receive(server_conn_sd, message, MAX_BUFFER_SIZE);
   	token = strtok(message, DELIMITER);
    sscanf(token, "%" SCNd16, &result);
    
    if (result != SUCCESS) {
    	// In case of error we print the error message
    	token = strtok(NULL, DELIMITER);
    	printf("%s\n", token);
    	goto register_cleanup;
    } else {
    	printf("Successfully registered\n");
    	strcpy(local_username, username);
    	is_username_set = true;
    }
    
    check_for_offline_messages(server_conn_sd);
register_cleanup:
    free(message);
}



static void
execute_who(int server_conn_sd)
{
    char *message = malloc(MAX_BUFFER_SIZE);
	if (message == NULL) {
		printf("Error during malloc(): out of memory\n");
		exit(-1);
	}
	
    sprintf(message, "%" PRId16, WHO);
    tcp_send(server_conn_sd, message);
    
    tcp_receive(server_conn_sd, message, MAX_BUFFER_SIZE);
    int16_t how_many;
    sscanf(message, "%" SCNd16, &how_many);
    
    printf("Registered clients:\n");
    for (; how_many > 0; --how_many) {
    	tcp_receive(server_conn_sd, message, MAX_BUFFER_SIZE);
    	printf("\t%s\n", message);
    }
    
    free(message);
}



static void
execute_quit(int server_conn_sd, int udp_sd)
{
    char message[MAX_MESSAGE_LENGTH];
    sprintf(message, "%" PRId16, QUIT);
    tcp_send(server_conn_sd, message);
    safe_close(server_conn_sd);
    safe_close(udp_sd);
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
			"%" PRId16 ";%s;%s (offline msg)>\n%s",
			SEND,
			dest_username,
			local_username,
			message);
	tcp_send(server_conn_sd, buffer);
	tcp_receive(server_conn_sd, buffer, MAX_BUFFER_SIZE);
	// We only need to show the response message, even if the offline send
	// failed we can't do anything about it
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
	char ip_address[INET_ADDRSTRLEN];
	uint16_t port_number;
	int16_t result_code;
	char *token = NULL;
	char *buffer = malloc(MAX_BUFFER_SIZE);
    if (buffer == NULL) {
    	printf("Error during malloc(): out of memory\n");
    	exit(-1);
    }
	
	// First we need to resolve the username
	sprintf(buffer, "%" PRId16 ";%s", RESOLVE_NAME, username);
	tcp_send(server_conn_sd, buffer);
	tcp_receive(server_conn_sd, buffer, MAX_BUFFER_SIZE);
	//printf("Received: %s\n", buffer);
	
	// We parse the result_code to decide what to do next
	token = strtok(buffer, DELIMITER);
	sscanf(token, "%" SCNd16, &result_code);
	
	if (result_code == USERNAME_NOT_FOUND) {
		printf("%s is not a registered username\n", username);
		goto send_cleanup;
	}
	
	// Either if we need to send an offline or direct message we need to compose it
	bool parsing_completed = parse_message(message, sizeof(message));
	if (!parsing_completed) {
		printf("Message too log. Max message size: %" PRId16 "\n", MAX_MESSAGE_LENGTH);
		goto send_cleanup;
	}
	
	if (result_code == USERNAME_NOT_ONLINE) {
		send_offline(server_conn_sd, username, message);
		goto send_cleanup;
	}
	
	// result_code == USERNAME_ONLINE, in this case the response from the server
	// included the username ip_address and port_number
	token = strtok(NULL, DELIMITER);
	strcpy(ip_address, token);
	token = strtok(NULL, DELIMITER);
	sscanf(token, "%" SCNu16, &port_number);
    
	sprintf(buffer, "%s (online msg)>\n%s", local_username, message);
	udp_send(udp_sd, buffer, ip_address, port_number);

send_cleanup:
	free(buffer);
}



void
initialize_chat_client(const char *ip, uint16_t port)
{
	strcpy(local_ip, ip);
	local_port = port;
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
	// Client side commands are always composed by "!<command> <username>"
	// (<username> is not required by every command)
    char username[MAX_USERNAME_LENGTH];
    int16_t command;

	// Client side command parsing
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
            execute_quit(server_conn_sd, udp_sd);
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
