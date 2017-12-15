#ifndef CHAT_APP_CLIENT_H
#define CHAT_APP_CLIENT_H



#include <inttypes.h>



void
set_ip_and_port(const char *ip, uint16_t port);



void
execute_command(int server_conn_sd, char* command);



#endif
