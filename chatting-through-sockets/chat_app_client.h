#ifndef CHAT_APP_CLIENT_H
#define CHAT_APP_CLIENT_H



#include <inttypes.h>



void
initialize_chat_client(const char *ip, uint16_t port);



void
execute_command(int server_conn_sd, char* command);



void
execute_help(void);



#endif
