#ifndef SAFE_TCP_H
#define SAFE_TCP_H


#include "safe_socket.h"



// Returns a TCP socket descritor,
// stops the process execution if something goes wrong
int
create_tcp_socket();



// Returns a passive TCP socket descritor bound to the specified ip adress and port number,
// stops the process execution if something goes wrong
int
create_passive_tcp_socket(const char *ip_address, uint16_t port_number, int backlog);



// Returns a new socket descriptor for the new TCP connection with the connecting host,
// stops the process execution if something goes wrong
// TODO: write a version which returns ip address and port number of the client
int
safe_accept(int passive_socket);



void
safe_connect(int socket_des, const char *ip_address, uint16_t port_number);



// First sends the message size as an uint16_t then the message,
// message must be a C string (null byte terminated)
// stops the process execution if something goes wrong
void
tcp_send(int socket_des, const void *message);



// Returns 0 if the other host has closed the connection
// keeps receiving until it has read exactly messager_length
// stops the process execution if message isn't big enough to store the
// "message" sent by the other host
int
tcp_receive(int socket_des, void *message, uint16_t message_length);



#endif