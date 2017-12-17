#include "safe_udp.h"
#include "safe_socket.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int
create_udp_socket(void)
{
	return create_socket(SOCK_DGRAM);
}



void
udp_send(int socket_des, const char *message, const char *ip_address,
		 uint16_t port_number)
{
	struct sockaddr_in dest_addr = create_addr_struct(ip_address, port_number);
	ssize_t result = sendto(socket_des,
							message,
							strlen(message) + 1,
							0,
							(struct sockaddr *)&dest_addr,
							sizeof(dest_addr));
	if (result == -1) {
		perror("Error during sendto():");
		exit(-1);
	}
}



void
udp_receive(int socket_des, char *message, uint16_t max_message_size)
{
	ssize_t result = recvfrom(socket_des,
							  message,
							  max_message_size,
							  0,
							  NULL,
							  NULL);
	if (result == -1) {
		perror("Error during recvfrom():");
		exit(-1);
	}
}
