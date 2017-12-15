#include "safe_tcp.h"
#include "safe_socket.h"
#include "utilities.h"
#include "chat_app_client.h"
#include "chat_app_sizes.h"

#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>



#define BUFFER_SIZE 128



void
check_cli_arguments(int argc, char *argv[])
{
    if (argc != 5) {
        printf("Correct usage ./msg_client <local_ip> <local_port> <server_ip> <server_port>\n");
        exit(-1);
    }
}



int
main(int argc, char *argv[])
{
    char buffer[MAX_MESSAGE_LENGTH];  // TODO: decide size for every buffer etc.
    //check_cli_arguments(argc, argv);
    set_sigpipe_handler();
    // char local_ip = argv[1];
    // uint16_t local_port = get_port_number(argv[2]);
    // char server_ip = argv[3];
    // uint16_t server_port = get_port_number(argv[4]);
    char server_ip[] = "127.0.0.1";
    uint16_t server_port = 8080;
    int server_conn_sd = create_tcp_socket();
    safe_connect(server_conn_sd, server_ip, server_port);
    
    char local_ip[] = "127.0.0.1";
    uint16_t local_port = 9000;
    set_ip_and_port(local_ip, local_port);
    // TODO: create and bind UDP socket
    
    int max_des = STDIN_FILENO;
    fd_set read_master;
    FD_ZERO(&read_master);
    FD_SET(max_des, &read_master);

    for (;;) {
        fd_set read_worker = read_master;
        safe_select(max_des + 1, &read_worker);
        for (int i = 0; i <= max_des; ++i) {
            if (FD_ISSET(i, &read_worker)) {
                if (i == STDIN_FILENO) {
                    // New user command
                    fgets(buffer, BUFFER_SIZE, stdin);
                    buffer[strlen(buffer) - 1] = '\0';    // removes trailing '\n'
                    execute_command(server_conn_sd, buffer);
                } else {
                    // Receiving an UDP message from another client
                }
                printf("\n");
            }
        }
    }

    return 0;
}
