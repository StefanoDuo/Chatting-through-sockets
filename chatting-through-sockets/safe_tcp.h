#ifndef SAFE_TCP_H
#define SAFE_TCP_H


#include "safe_socket.h"
#include <stdbool.h>



// Returns a TCP socket descritor,
// stops the process execution if something goes wrong
int
create_tcp_socket(void);



/* Returns a passive TCP socket descritor bound to the specified ip adress and port number,
 * stops the process execution if something goes wrong
 */
int
create_passive_tcp_socket(const char *ip_address, uint16_t port_number, int backlog);



/* Returns a new socket descriptor for the new TCP connection with the connecting host,
 * stops the process execution if something goes wrong
 */
int
safe_accept(int passive_socket);



void
safe_connect(int socket_des, const char *ip_address, uint16_t port_number);



// First sends the message size as an uint16_t then the message,
// message must be a C string (null byte terminated)
// stops the process execution if something goes wrong
void
tcp_send(int socket_des, const char *message);



/* message_max_legnth specifies the size of the message parameter, because the
 * application protocol used specifies the message exact length before sending
 * it we can check if message is big enough to store it, if it's not tcp_receive()
 * will stop the process execution.
 *
 * Returns false if the connection has been closed by the other host, true otherwise.
 * message will contain a null byte terminated string.
 */
bool
tcp_receive(int socket_des, char *message, uint16_t message_max_length);



#endif