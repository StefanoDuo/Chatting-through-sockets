#ifndef SAFE_UDP_H
#define SAFE_UDP_H



#include <inttypes.h>



int
create_udp_socket(void);



void
udp_send(int socket_des, const char *message, const char *ip, uint16_t port);



void
udp_receive(int socket_des, char *message, uint16_t max_message_size);



#endif
