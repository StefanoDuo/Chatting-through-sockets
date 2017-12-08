#ifndef CHAT_APP_SERVER_H
#define CHAT_APP_SERVER_H

#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include "safe_socket.h"
#include "safe_tcp.h"



#define MAX_MESSAGE_LENGTH 256



// client_socket_des is used as a client_id in all the functions
// TODO: decide if show only server_request() as the app interface
// TODO: move all the includes in every .h file inside the .c file



void
register_client_as(int client_socket_des, const char *username, const char *ip_address, uint16_t port_number);



void
deregister_client(int client_socket_des);



void
send_registered_users(int client_socket_des);



void
store_offline_message(int client_socket_des);



void
retrieve_offline_message(int client_socket_des, char *requesting_username);



bool
serve_request(int client_socket_des);



#endif