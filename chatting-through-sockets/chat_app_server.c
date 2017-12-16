#include "chat_app_server.h"
#include "server_command_parser.h"
#include "safe_socket.h"
#include "safe_tcp.h"
#include "chat_app_sizes.h"
#include "commands_format.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <stdlib.h>



#define MAX_REGISTERED_CLIENTS 128
#define MAX_OFFLINE_MESSAGES 256



struct client {
    bool is_in_use;
    bool is_online;
    char username[MAX_USERNAME_LENGTH];
    char ip_address[INET_ADDRSTRLEN];
    uint16_t port_number;
    int socket_des;
};



struct offline_message {
    char message[MAX_MESSAGE_LENGTH];
    char sender_username[MAX_USERNAME_LENGTH];
    char receiver_username[MAX_USERNAME_LENGTH];
};



static struct client reg_clients[MAX_REGISTERED_CLIENTS];
static int curr_reg_clients = 0;



static struct offline_message offline_messages[MAX_OFFLINE_MESSAGES];
static int stored_messages = 0;



/****************************************************************
 ***** FUNCTIONS USED TO HANDLE REGISTERED CLIENTS STRUCTURE ****
 ****************************************************************/



static inline bool
is_index_in_use(int16_t index)
{
    return reg_clients[index].is_in_use;
}



static inline bool
is_index_online(int index)
{
    // return reg_clients[index].socket_des == -1;
    return reg_clients[index].is_online;
}



static inline bool
is_username_register_full(void)
{
    return curr_reg_clients == MAX_REGISTERED_CLIENTS;
}



// Returns the username index inside of the register, -1 if username doesn't exists
static int16_t
get_username_index(const char *username)
{
    for (int16_t i = 0; i < curr_reg_clients; ++i) {
        if (strcmp(reg_clients[i].username, username) == 0 && is_index_in_use(i))
            return i;
    }
    return USERNAME_NOT_FOUND;
}



static int16_t
get_registered_users_number(void)
{
    int16_t how_many = 0;
    for (int16_t i = 0; i < curr_reg_clients; ++i) {
        if(is_index_in_use(i))
            ++how_many;
    }

    return how_many;
}



// Returns the client register index, -1 if the client isn't registered
static int16_t
get_client_index(int client_socket_des)
{
    for (int16_t i = 0; i < curr_reg_clients; ++i) {
        if (reg_clients[i].socket_des == client_socket_des && is_index_in_use(i) && is_index_online(i))
            return i;
    }

    return -1;
}



// If return value is >= 0 username will contain the client associated username (through registration)
// returns -1 if the specified client isn't registered
static int16_t
get_client_username(int client_socket_des, char *username)
{
    int16_t index = get_client_index(client_socket_des);
    if (index == -1)
        return -1;

    strcpy(username, reg_clients[index].username);
    return index;
}


// used only for RESOLVE_NAME therefore it directly uses client --> server error codes
static int16_t
get_ip_and_port_from_username(const char *username, char *ip_address, uint16_t *port_number)
{
    int16_t index = get_username_index(username);
    if (index == USERNAME_NOT_FOUND)
        return USERNAME_NOT_FOUND;

    if (!is_index_online(index))
        return USERNAME_NOT_ONLINE;

    strcpy(ip_address, reg_clients[index].ip_address);
    *port_number = reg_clients[index].port_number;
    return index;
}


static int16_t
get_available_index()
{
    if (is_username_register_full())
        return -1;
    return curr_reg_clients++;
}


static bool
insert_username_registration(int16_t index, const char *username, const char *ip_address, uint16_t port_number, int client_socket_des)
{
    if (index == -1) {
        index = get_available_index();
        if (index == -1)
            return false;
    }
    strcpy(reg_clients[index].username, username);
    strcpy(reg_clients[index].ip_address, ip_address);
    reg_clients[index].port_number = port_number;
    reg_clients[index].socket_des = client_socket_des;
    reg_clients[index].is_in_use = true;
    reg_clients[index].is_online = true;
    return true;
}



static void
remove_index_registration(int16_t index)
{
    reg_clients[index].is_in_use = false;
}



static void
set_index_offline(int16_t index)
{
    reg_clients[index].is_online = false;
}



/**************************************************************
 ***** FUNCTIONS USED TO HANDLE OFFLINE MESSAGE STRUCTURES ****
 **************************************************************/



static inline int
is_offline_storage_full(void)
{
    return stored_messages == MAX_OFFLINE_MESSAGES;
}



/****************************************************
 ***** FUNCTIONS USED TO SERVE CLIENTS' REQUESTS ****
 ****************************************************/



void
register_client_as(int client_socket_des, const char *username, const char *ip_address, uint16_t port_number)
{
    // TODO: decide whether to remove or not (client alredy registered)
    // we can move this check on the client side (less secure but easier)
    int16_t index = get_client_index(client_socket_des);
    if (index != -1) {
        tcp_send(client_socket_des, "0;You are alredy registered");
        return;
    }

    index = get_username_index(username);
    // if the username is alredy registered we can reregister only if it is offline
    if (index >= 0 && is_index_online(index)) {
        tcp_send(client_socket_des, "0;Username alredy in use");
        return;
    }

    bool success = insert_username_registration(index, username, ip_address, port_number, client_socket_des);
    if (!success) {
    	tcp_send(client_socket_des, "0;Register full");
    }
    tcp_send(client_socket_des, "1;Success");
}



void
set_client_offline(int client_socket_des)
{
    // we don't send anything back because we expect the client to close the connection after
    // sending that request
    int16_t index = get_client_index(client_socket_des);
    if (index == -1)
        return;

    set_index_offline(index);
}



void
deregister_client(int client_socket_des)
{
    // TODO: decide whether to remove or not (check done on client side)
    int index = get_client_index(client_socket_des);
    if (index == -1) {
        tcp_send(client_socket_des, "0;You are not registered");
        return;
    }

    remove_index_registration(index);
}



void
send_registered_users(int client_socket_des)
{
    char buffer[MAX_MESSAGE_LENGTH];
    int16_t how_many = get_registered_users_number();
    sprintf(buffer, "%" SCNd16, how_many);
    tcp_send(client_socket_des, buffer);
    for (int16_t i = 0; i < curr_reg_clients; ++i) {
        if (is_index_in_use(i)) {
        	
            sprintf(buffer,
            		"%s(%s)",
            		reg_clients[i].username,
            		(is_index_online(i) ? "online" : "offline"));
            tcp_send(client_socket_des, buffer);
        }
    }
}



void
store_offline_message(int client_socket_des)
{
    // TODO: check if full
    // TODO: check if nicknames exists
    uint16_t message_max_size = MAX_USERNAME_LENGTH + MAX_MESSAGE_LENGTH + 1;
    char *message = malloc(message_max_size);
    if (message == NULL) {
        printf("Out of memory\n");
        exit(-1);
    }

    char *buffer;

    tcp_receive(client_socket_des, message, message_max_size);
    strtok(message, ";");

    // parse the receiver username
    // TODO: check if strtok works
    buffer = strtok(NULL, ";");
    strcpy(offline_messages[stored_messages].receiver_username, buffer);

    buffer = strtok(NULL, ";");
    strcpy(offline_messages[stored_messages].message, buffer);

    char sender_username[MAX_MESSAGE_LENGTH];
    int result;
    // decide if we need to implement server side check for send without being registered
    result = get_client_username(client_socket_des, sender_username);
    if (result == -1) {
        // TODO: handle this case
    }

    strcpy(offline_messages[stored_messages].sender_username, sender_username);
    ++stored_messages;
}



void
retrieve_offline_message(int client_socket_des, char *requesting_username)
{
    char *response = malloc(MAX_USERNAME_LENGTH + MAX_MESSAGE_LENGTH + 1);
    if (response == NULL) {
        printf("Out of memory\n");
        exit(-1);
    }
    // First we calculate and tell the client how many offline messages he has received
    int how_many = 0;
    for (int i = 0; i < stored_messages; ++i) {
        if (strcmp(offline_messages[i].receiver_username, requesting_username) == 0)
            ++how_many;
    }
    sprintf(response, "%d", how_many);
    tcp_send(client_socket_des, response);

    // Then we send those messages
    for (int i = 0; i < stored_messages; ++i) {
        if (strcmp(offline_messages[i].receiver_username, requesting_username) == 0) {
            // strcpy(response, offline_messages[i].sender_username);
            // strcat(response, ";");
            // strcat(response, offline_messages[i].message);
            sprintf(response, "%s;%s", offline_messages[i].sender_username, offline_messages[i].message);
            tcp_send(client_socket_des, response);
        }
    }

    free(response);
}



static void
resolve_name(int client_socket_des, const char *username)
{
    char message[MAX_MESSAGE_LENGTH];
    char ip_address[INET_ADDRSTRLEN];
    uint16_t port_number;
    int16_t index = get_ip_and_port_from_username(username, ip_address, &port_number);

    if (index < 0)
    	sprintf(message, "%" PRId16, index);
    else 
		sprintf(message, "%" PRId16 ";%s;%" PRIu16, SUCCESS, ip_address, port_number);
	printf("RESOLVE_NAME response: %s\n", message);
    tcp_send(client_socket_des, message);
}



bool
serve_request(int client_socket_des)
{
    // General request format: "command;username;ip_address;port_number\0"
    const uint16_t max_message_size = COMMAND_MAX_LENGTH + DELIMITER_LENGTH + MAX_USERNAME_LENGTH
        + DELIMITER_LENGTH + INET_ADDRSTRLEN + DELIMITER_LENGTH + PORT_NUMBER_MAX_LENGTH + 1;
    char *message = malloc(max_message_size);
    if (message == NULL) {
        printf("Error during malloc(): out of memory\n");
        exit(-1);
    }

    int16_t command;
    uint16_t port_number;
    char ip_address[INET_ADDRSTRLEN];
    char username[MAX_USERNAME_LENGTH];
    char offline_message[MAX_MESSAGE_LENGTH];
    bool is_conn_open = tcp_receive(client_socket_des, message, max_message_size);
    if (!is_conn_open) {
    	set_client_offline(client_socket_des);
        goto cleanup;
    }


    bool success = parse_message(message, &command, username, ip_address, &port_number, offline_message);
    if (!success) {
        printf("Error while parsing: unknown command or wrong syntax\n");
        tcp_send(client_socket_des, "0;Unknown command or wrong syntax");
        goto cleanup;
    }

    switch(command) {
        case REGISTER:
            printf("REGISTER request\n");
            register_client_as(client_socket_des, username, ip_address, port_number);
            break;
        case WHO:
            printf("WHO request\n");
            send_registered_users(client_socket_des);
            break;
        case QUIT:
            printf("QUIT request\n");
            set_client_offline(client_socket_des);
            is_conn_open = false;
            break;
        case DEREGISTER:
            printf("DEREGISTER request\n");
            deregister_client(client_socket_des);
            break;
        case RESOLVE_NAME:
            printf("RESOLVE_NAME request\n");
            resolve_name(client_socket_des, username);
            break;
    }

cleanup:
    free(message);
    return is_conn_open;
}
