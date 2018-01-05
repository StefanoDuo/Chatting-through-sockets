#ifndef CHAT_APP_SERVER_H
#define CHAT_APP_SERVER_H



#include <stdbool.h>



// Serves a pending request from client_socket_des
bool
serve_request(int client_socket_des);



#endif
