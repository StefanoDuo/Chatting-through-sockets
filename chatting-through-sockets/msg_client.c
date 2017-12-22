#include "safe_tcp.h"
#include "safe_udp.h"
#include "safe_socket.h"
#include "utilities.h"
#include "chat_app_client.h"
#include "chat_app_sizes.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>



void
check_cli_arguments(int argc, char *argv[])
{
    if (argc != 5) {
        printf("Correct usage ./msg_client <local_ip> <local_port> <server_ip> <server_port>\n");
        exit(-1);
    }
}



// defined in chat_app_client.c
extern char local_username[MAX_USERNAME_LENGTH];



int
main(int argc, char *argv[])
{
    char *buffer = malloc(MAX_BUFFER_SIZE);
    if (buffer == NULL) {
    	printf("Error during malloc(): out of memory\n");
    	exit(-1);
    }
    
    check_cli_arguments(argc, argv);
    set_sigpipe_handler();
    
    char *local_ip = argv[1];
    uint16_t local_port = get_port_number(argv[2]);
    char *server_ip = argv[3];
    uint16_t server_port = get_port_number(argv[4]);
    
    int server_conn_sd = create_tcp_socket();
    safe_connect(server_conn_sd, server_ip, server_port);
    printf("\nSuccessfully connected to %s:%" PRIu16 "\n", server_ip, server_port);
    
    initialize_chat_client(local_ip, local_port);
    int udp_sd = create_udp_socket();
    safe_bind(udp_sd, local_ip, local_port);
    printf("Listening for direct messages on %s:%" PRIu16 "\n\n", local_ip, local_port);
    execute_help();
    printf("\n");
    
    int max_des = MAX(STDIN_FILENO, udp_sd);
    fd_set read_master;
    FD_ZERO(&read_master);
    FD_SET(STDIN_FILENO, &read_master);
    FD_SET(udp_sd, &read_master);

    for (;;) {
        printf("%s> ", local_username);
        fflush(stdout);
        
        fd_set read_worker = read_master;
        safe_select(max_des + 1, &read_worker);
        
        for (int i = 0; i <= max_des; ++i) {
            if (FD_ISSET(i, &read_worker)) {
                if (i == STDIN_FILENO) {
                    // New user command
                    fgets(buffer, MAX_BUFFER_SIZE, stdin);
                    buffer[strlen(buffer) - 1] = '\0';    // removes trailing '\n'
                    execute_command(server_conn_sd, udp_sd, buffer);
                } else {
                	// New direct message
                	udp_receive(i, buffer, MAX_BUFFER_SIZE);
                	// \33[2K erases the current line
                	// \r brings the cursor at the beginning of the line
                	printf("\33[2K\r%s", buffer);
                }
            }
        }
    }

    return 0;
}
