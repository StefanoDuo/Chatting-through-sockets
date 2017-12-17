#include "safe_tcp.h"
#include "safe_socket.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>



static void
safe_listen(int socket_des, int backlog)
{
    int result = listen(socket_des, backlog);
    if (result == -1) {
        perror("Error during listen()");
        exit(-1);
    }
}



int
create_tcp_socket(void)
{
    return create_socket(SOCK_STREAM);
}



int
create_passive_tcp_socket(const char *ip_address, uint16_t port_number, int backlog)
{
    int socket_des = create_tcp_socket();
    safe_bind(socket_des, ip_address, port_number);
    safe_listen(socket_des, backlog);
    return socket_des;
}



int
safe_accept(int passive_socket)
{
    int connection_socket_des = accept(passive_socket, NULL, NULL);
    if (connection_socket_des == -1) {
        perror("Error during accept()");
        exit(-1);
    }
    return connection_socket_des;
}



void
safe_connect(int socket_des, const char *ip_address, uint16_t port_number)
{
    struct sockaddr_in addr = create_addr_struct(ip_address, port_number);
    int result = connect(socket_des, (struct sockaddr *)&addr, sizeof(addr));
    if (result == -1) {
        perror("Error during connect()");
        exit(-1);
    }
}


static void
safe_send(int socket_des, const void *message, uint16_t message_length)
{
    uint16_t bytes_sent = 0;
    /* send might send less than message_length if it blocks and the process receives
     * a signal, to avoid sending less than wanted we wrap it into a while
     * https://stackoverflow.com/questions/19697218/can-send-on-a-tcp-socket-return-0-and-length
     *
     * If we send on a closed "connection" we receive a SIGPIPE signal
     */
    while (bytes_sent < message_length) {
    	// result contains the exact number of bytes sent
        int result = send(socket_des, message, message_length, 0);
        if (result < 0) {
            perror("Error during send()");
            exit(-1);
        }
        bytes_sent += result;
        message += result;
    }
}


/* Implements the application protocol, i.e. first we send the size of the message
 * as an uint16_t then we send the real message
 */
void
tcp_send(int socket_des, const char *message)
{
    uint16_t message_length = strlen(message) + 1;  // we include the terminating null byte
    uint16_t network_message_length = htons(message_length);
    safe_send(socket_des, &network_message_length, sizeof(uint16_t));
    safe_send(socket_des, message, message_length);
}



static bool
safe_receive(int socket_des, void *message, uint16_t message_length)
{
    uint16_t bytes_received = 0;
    // We need the while for the same reason illustrated inside safe_send()
    while (bytes_received < message_length) {
        int result = recv(socket_des, message, message_length, 0);
        if (result == -1) {
            perror("Error during recv()");
            exit(-1);
        }
        if (result == 0)
        	// Connection has been closed by the other host
            return false;
        bytes_received += result;
        message += result;
    }
    return true;
}



bool
tcp_receive(int socket_des, char *message, uint16_t message_max_length)
{
    uint16_t network_message_length = 0;
    bool is_conn_open = safe_receive(socket_des, &network_message_length, sizeof(uint16_t));
    if (!is_conn_open)
        return false;

    uint16_t host_message_length = ntohs(network_message_length);
    if (host_message_length > message_max_length) {
        printf("Error during recv(): message too big\n");
        exit(-1);
    }

    return safe_receive(socket_des, message, host_message_length);
}
