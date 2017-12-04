#include "chat_app_server.h"



#define MAX_USERNAME_LENGTH 36
#define MAX_REGISTERED_CLIENTS 128
#define MAX_OFFLINE_MESSAGES 256



static struct client {
    char username[MAX_USERNAME_LENGTH];
    char ip_address[INET_ADDRSTRLEN];
    uint16_t port_number;
    int socket_des;
};



static struct offline_message {
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



static inline int
is_username_register_full()
{
    return curr_reg_clients == MAX_REGISTERED_CLIENTS;
}



// Returns the username index inside of the register, -1 if username doesn't exists
static int
get_username_index(const char *username)
{
    for (int i = 0; i < curr_reg_clients; ++i) {
        if (strcmp(reg_clients[i].username, username) == 0)
            return i;
    }
    return -1;
}



// Requests the username index to be sure we're calling it on a registered username
// therefore you need to call get_username_index() first to get the index
// Returns -1 if offline, >= 0 otherwise
static inline int
is_username_online(int index)
{
    return reg_clients[index].socket_des == -1;
}



// Returns the client register index, -1 if the client isn't registered
static int
get_client_index(int client_socket_des)
{
    for (int i = 0; i < curr_reg_clients; ++i) {
        if (reg_clients[i].socket_des == client_socket_des)
            return i;
    }

    return -1;
}



// If return value is >= 0 username will contain the client associated username (through registration)
// returns -1 if the specified client isn't registered
static int
get_client_username(int client_socket_des, char *username)
{
    int index = get_client_index(client_socket_des);
    if (index == -1)
        return -1;

    strcpy(username, reg_clients[index].username);
    return index;
}



static int
get_ip_and_port_from_username(const char *username, char *ip_address, uint16_t *port_number)
{
    int index = get_username_index(username);
    if (index == -1)
        return -1;

    strcpy(ip_address, reg_clients[index].ip_address);
    *port_number = reg_clients[index].port_number;
    return index;
}



static void
insert_username_registration(int index, char *username, char *ip_address, uint16_t port_number, int client_socket_des)
{
    if (index == -1) {
        index = curr_reg_clients;
        ++curr_reg_clients;
    }
    strcpy(reg_clients[index].username, username);
    strcpy(reg_clients[index].ip_address, ip_address);
    reg_clients[index].port_number = port_number;
    reg_clients[index].socket_des = client_socket_des;
}



static void
remove_username_registration(int index)
{
    strcpy(reg_clients[index].username, "");
    strcpy(reg_clients[index].ip_address, "");
    reg_clients[index].port_number = -1;
    reg_clients[index].socket_des = -1;
}



/**************************************************************
 ***** FUNCTIONS USED TO HANDLE OFFLINE MESSAGES STRUCTURE ****
 **************************************************************/



static inline int
is_offline_storage_full()
{
    return stored_messages == MAX_OFFLINE_MESSAGES;
}



/****************************************************
 ***** FUNCTIONS USED TO SERVE CLIENTS' REQUESTS ****
 ****************************************************/



int
serve_request(int client_socket_des)
{
    char message[128];
    uint16_t message_size = 128;
    uint16_t command;
    char username[MAX_USERNAME_LENGTH];
    tcp_receive(client_socket_des, message, message_size);
    // https://stackoverflow.com/questions/12120426/how-to-print-uint32-t-and-uint16-t-variables-value
    // -Wall still gives a warning tho
    sscanf(message, "%u;%s", &command, username);
    printf("%s\n", message);
    printf("%u;%s\n", command, username);

    // TODO: write switch case on command variable
    // switch(command) {
    //     case 0:

    //         break;
    //     case 1:
    //         break;
    // }
    return -1;
}



int
register_client_as(int client_socket_des, char *username, char *ip_address, uint16_t port_number)
{
    if (is_username_register_full()) {
        // TODO: handle full register
        return -1;
    }

    // TODO: decide whether to remove or not
    // we can probably move this check on the client side (less secure but easier)
    int index = get_client_index(client_socket_des);
    if (index != -1) {
        // TODO: handle client alredy registered
        return -1;
    }

    index = get_username_index(username);
    if (index >= 0 && is_username_online(index)) {
        // TODO: handle username alredy online
        return -1;
    }

    insert_username_registration(index, username, ip_address, port_number, client_socket_des);
    // TODO: send success message to client
    return 0;
}



int
deregister_client(int client_socket_des)
{
    // we can probably move this check on the client side (less secure but easier)
    int index = get_client_index(client_socket_des);
    if (index == -1) {
        // TODO: handle deregister request from non registered client
        // (comunicate unsuccess to the client)
        return -1;
    }

    remove_username_registration(index);
    // TODO: send success message to client
    return -1;
}



int
send_registered_users(int client_socket_des)
{
    return -1;
}



int
store_offline_message(int client_socket_des)
{
    // TODO: check if full
    // TODO: check if nicknames exists
    uint16_t message_max_size = MAX_USERNAME_LENGTH + MAX_MESSAGE_LENGTH + 1;
    char *message = malloc(message_max_size);
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
    return 0;
}



int
retrieve_offline_message(int client_socket_des, char *requesting_username)
{
    char *response = malloc(MAX_USERNAME_LENGTH + MAX_MESSAGE_LENGTH + 1);
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
    return 0;
}