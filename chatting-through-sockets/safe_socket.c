#include "safe_socket.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>



int
create_socket(int type)
{
    int socket_des = socket(AF_INET, type, 0);
    if(socket_des == -1) {
        perror("Error during socket()");
        exit(-1);
    }
    return socket_des;
}



void
safe_bind(int socket_des, const char *ip_address, uint16_t port_number)
{
    struct sockaddr_in addr = create_addr_struct(ip_address, port_number);
    int result = bind(socket_des, (struct sockaddr *)&addr, sizeof(addr));
    if (result == -1) {
        perror("Error during bind()");
        exit(-1);
    }
}



void
safe_close(int socket_des)
{
    int result = close(socket_des);
    if (result == -1) {
        perror("Error during close()");
        exit(-1);
    }
}



void
safe_select(int max_des, fd_set *reader_set)
{
    int result = select(max_des, reader_set, NULL, NULL, NULL);
    if (result == -1) {
        perror("Error during select()");
        exit(-1);
    }
}



struct sockaddr_in
create_addr_struct(const char *ip_address, uint16_t port_number)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_number);

    if(ip_address == NULL) {
        addr.sin_addr.s_addr = INADDR_ANY;
    }
    else {
        int result = inet_pton(AF_INET, ip_address, &addr.sin_addr);
        if (result == -1) {
            printf("Error while translating the following ip address %s\n", ip_address);
            exit(-1);
        }
    }

    return addr;
}