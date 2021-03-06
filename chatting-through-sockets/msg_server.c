#include "chat_app_server.h"
#include "safe_tcp.h"
#include "safe_socket.h"
#include "utilities.h"

#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>



#define BACKLOG 8



void
check_cli_arguments(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Correct usage ./msg_server <port_number>\n");
        exit(-1);
    }
}



int
main(int argc, char *argv[])
{
    check_cli_arguments(argc, argv);
    set_sigpipe_handler();
    
    uint16_t port_number = get_port_number(argv[1]);
    char ip_address[INET_ADDRSTRLEN];
    int server_socket = create_passive_tcp_socket(NULL, port_number, BACKLOG);
    printf("\nListening for incoming connections on :%" PRIu16 "\n\n", port_number);

    int max_des = server_socket;
    fd_set read_master;
    FD_ZERO(&read_master);
    FD_SET(server_socket, &read_master);

    for (;;) {
        int client_socket;
        fd_set read_worker = read_master;
        safe_select(max_des + 1, &read_worker);
        for (int i = 0; i <= max_des; ++i) {
            if (FD_ISSET(i, &read_worker)) {
                if (i == server_socket) {
                    // New TCP connection request
                    client_socket = safe_accept(server_socket, ip_address, &port_number);
                    printf("New TCP connection from %s:%" PRIu16 "\n", ip_address, port_number);
                    FD_SET(client_socket, &read_master);
                    max_des = MAX(max_des, client_socket);
                } else {
                    // An alredy connected client is requesting something
                    bool is_conn_open = serve_request(i);
                    if (!is_conn_open) {
                    	safe_getpeername(i, ip_address, &port_number);
                        printf("Connection closed by the client %s:%" PRIu16 "\n",
                        	   ip_address,
                        	   port_number);
                        FD_CLR(i, &read_master);
                        safe_close(i);
                    }
                }
            }
        }
    }

    return 0;
}
