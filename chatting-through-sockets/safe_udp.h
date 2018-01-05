#ifndef SAFE_UDP_H
#define SAFE_UDP_H



#include <inttypes.h>



/* Returns an UDP socket descritor
 *
 * Stops the process execution if something goes wrong
 */
int
create_udp_socket(void);



/* message must be a C string
 *
 * Stops the process execution if something goes wrong
 */
void
udp_send(int socket_des, const char *message, const char *ip, uint16_t port);



/* message will contain a C string
 *
 * Stops the process execution if something goes wrong
 */
void
udp_receive(int socket_des, char *message, uint16_t max_message_size);



#endif
