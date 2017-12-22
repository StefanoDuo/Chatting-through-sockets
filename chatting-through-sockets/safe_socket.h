#ifndef SAFE_SOCKET_H
#define SAFE_SOCKET_H



#include <inttypes.h>
#include <sys/select.h>
#include <netinet/in.h>



// Returns a socket descriptor of the specified type,
// stops the process execution if something goes wrong
int
create_socket(int type);



// Binds socket_des to the specified ip address and port number,
// stops the process execution if something goes wrong
void
safe_bind(int socket_des, const char *ip_address, uint16_t port_number);



// Closes socket_des,
// stops the process execution if something goes wrong
void
safe_close(int socket_des);



void
safe_select(int max_des, fd_set *reader_set);



// Translates the dotted decimal ip_address and port_number into a sockaddr_in struct
// stops the process execution if something goes wrong (e.g. ip_address is not a correct
// dotted decimal ip address)
struct sockaddr_in
create_addr_struct(const char *ip_address, uint16_t port_number);



void
addr_from_struct(struct sockaddr_in addr, char *ip_address, uint16_t *port_number);



void
safe_getpeername(int socket_des, char *ip_address, uint16_t *port_number);



#endif
