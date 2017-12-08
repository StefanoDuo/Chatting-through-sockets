#include "safe_tcp.h"
#include "safe_socket.h"
#include "command_parser.h"
#include <inttypes.h>

#define BUFFER_SIZE 20

#define MAX_USERNAME_LENGTH 36
#define COMMAND_MAX_LENGTH 5
#define PORT_NUMBER_MAX_LENGTH 5
#define DELIMITER_LENGTH 1
#define MAX_MESSAGE_LENGTH 256

int main() {
    int port_number = 8080;

    int server_socket = create_passive_tcp_socket(NULL, port_number, 8);
    printf("Sono in ascolto sulla porta %d\n", port_number);
    
    int connection_socket = safe_accept(server_socket);
    printf("Ho accettato una nuova connessione\n");
    
    for(;;) {
        const uint16_t max_message_size = COMMAND_MAX_LENGTH + DELIMITER_LENGTH + MAX_USERNAME_LENGTH
            + DELIMITER_LENGTH + INET_ADDRSTRLEN + DELIMITER_LENGTH + PORT_NUMBER_MAX_LENGTH + 1;
        char *msg = malloc(max_message_size);
        if (msg == NULL) {
            printf("Out of memory\n");
            exit(-1);
        }
        
        int result = tcp_receive(connection_socket, msg, max_message_size);
        if (result == 0) {
            printf("La connessione e' stata chiusa\n");
            break;
        }
        printf("Ho ricevuto: %s\n", msg);

        uint16_t command;
        uint16_t port_number;
        char ip_address[INET_ADDRSTRLEN];
        char username[MAX_USERNAME_LENGTH];
        char offline_message[MAX_MESSAGE_LENGTH];
        
        bool success = parse_message(msg, &command, username, ip_address, &port_number, offline_message);

        char answer[2];
        if (success)
            strcpy(answer, "1");
        else
            strcpy(answer, "0");
        
        tcp_send(connection_socket, answer);
        printf("Invio: %s\n", answer);
    }
    
    close(connection_socket);
    return 0;
}