#include "safe_tcp.h"

int
create_tcp_socket()
{
    return create_socket(SOCK_STREAM);
}


void
safe_listen(int socket_des, int backlog)
{
    int result = listen(socket_des, backlog);
    if (result == -1) {
        perror("Error during listen()");
        exit(-1);
    }
}


int
create_passive_tcp_socket(char *ip_address, uint16_t port_number, int backlog)
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
        return 1;
    }
    return connection_socket_des;
}


void
safe_send(int socket_des, void *message, uint16_t message_length)
{
    uint16_t bytes_sent = 0;
    // send might send less than length if it blocks and the process receives a signal
    // https://stackoverflow.com/questions/19697218/can-send-on-a-tcp-socket-return-0-and-length
    while (bytes_sent < message_length) {
        int result = send(socket_des, message, message_length, 0);
        if (result < 0) {
            perror("Error during send()");
            exit(-1);
        }
        // Decidere se implementare error checking per errno == ECONNRESET
        // cioe' se l'altro host ha deciso di resettare la connessione senza fare
        // il corretto hand shaking ma senza neanche scomparire senza dire niente
        // o per errno == EPIPE cioe' quando la connessione viene chiusa con il corretto handshake
        // https://stackoverflow.com/questions/15406097/writing-on-a-tcp-socket-closed-by-the-peer
        bytes_sent += result;
    }
}


void
tcp_send(int socket_des, void *message)
{
    uint16_t message_length = strlen(message) + 1;
    uint16_t network_message_length = htons(message_length);
    safe_send(socket_des, &network_message_length, sizeof(uint16_t));
    safe_send(socket_des, message, message_length);
}


// returns 0 if the other host has closed the connection
int
safe_receive(int socket_des, void *message, uint16_t message_max_length)
{
    int result = recv(socket_des, message, message_max_length, 0);
    if(result == -1) {
        perror("Error during recv()");
        return 1;
    }
    return result;
}


// returns 0 if the other host has closed the connection
int
tcp_receive(int socket_des, void *message, uint16_t message_max_length)
{
    uint16_t network_message_length = 0;
    int result = safe_receive(socket_des, &network_message_length, sizeof(uint16_t));
    if (result == 0)
        return result;

    uint16_t host_message_length = ntohs(network_message_length);
    printf("%u\n", host_message_length);
    if (host_message_length > message_max_length) {
        printf("Error during recv(): message too big\n");
        exit(-1);
    }

    return safe_receive(socket_des, message, host_message_length);
}