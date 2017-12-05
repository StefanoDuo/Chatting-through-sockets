#include "chat_app_server.h"
#include "utilities.h"
#include "safe_tcp.h"
#include <sys/select.h>
#include <stdbool.h>
#include <inttypes.h>


#define MAX(a, b) ((a) > (b) ? (a) : (b))
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
    uint16_t port_number = get_port_number(argv[1]);
    int server_socket = create_passive_tcp_socket(NULL, port_number, BACKLOG);
    printf("Listening for incoming connections on port number %u\n", port_number);

    int max_des = server_socket;
    fd_set read_master;
    FD_ZERO(&read_master);
    FD_SET(server_socket, &read_master);

    for (;;) {
        fd_set read_worker = read_master;
        int client_socket;
        safe_select(max_des + 1, &read_worker);
        printf("Something happened ...\n");
        for (int i = 0; i <= max_des; ++i) {
            if (FD_ISSET(i, &read_worker)) {
                if (i == server_socket) {
                    // New TCP connection request
                    printf("Accepting a new TCP connection\n");
                    client_socket = safe_accept(server_socket);
                    FD_SET(client_socket, &read_master);
                    max_des = MAX(max_des, client_socket);
                } else {
                    // An alredy connected client is requesting something
                    printf("Handling a request from a connected client\n");
                    bool is_conn_closed = serve_request(i);
                    if (is_conn_closed) {
                        FD_CLR(i, &read_master);
                        safe_close(i);
                    }
                }
            }
        }
    }

    return 0;
}