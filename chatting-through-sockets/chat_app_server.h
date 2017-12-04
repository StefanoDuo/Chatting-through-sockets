#ifndef CHAT_APP_SERVER_H
#define CHAT_APP_SERVER_H

#include <string.h>
#include "safe_socket.h"
#include "safe_tcp.h"

#define MAX_MESSAGE_LENGTH 256

int
register_client_as(int client_socket_des, char *username, char *ip_address, uint16_t port_number);

int
deregister_client(int client_socket_des);

int
send_registered_users(int client_socket_des);

int
store_offline_message(int client_socket_des);

int
retrieve_offline_message(int client_socket_des, char *requesting_username);

int
serve_request(int client_socket_des);

#endif