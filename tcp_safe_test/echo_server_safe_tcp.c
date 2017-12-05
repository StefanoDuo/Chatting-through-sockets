#include "safe_tcp.h"
#include "safe_socket.h"

#define BUFFER_SIZE 20

int main() {
    int port_number = 8080;
    char msg[BUFFER_SIZE];
    char username[BUFFER_SIZE] = "stuff";
    uint16_t command;

    int server_socket = create_passive_tcp_socket(NULL, port_number, 8);
    printf("Sono in ascolto sulla porta %d\n", port_number);
    
    int connection_socket = safe_accept(server_socket);
    printf("Ho accettato una nuova connessione\n");
    
    for(;;) {
        int result = tcp_receive(connection_socket, msg, sizeof(msg));
        if (result == 0) {
            printf("La connessione e' stata chiusa\n");
            break;
        }
        printf("Ho ricevuto: %s", msg);

        // sscanf "parses" until msg ends or a character can't be matched with the format string
        // https://stackoverflow.com/questions/12120426/how-to-print-uint32-t-and-uint16-t-variables-value
        // https://stackoverflow.com/questions/33675163/what-is-the-correct-and-portable-clang-gcc-modifier-for-uint16-t-in-sscanf
        // http://en.cppreference.com/w/cpp/types/integer#Format_constants_for_the_std::fscanf_family_of_functions
        sscanf(msg, "%" SCNu16 ";%s", &command, username);
        printf("%" PRIu16 ";%s\n", command, username);

        if (strcmp(msg, "bye\n") == 0)
            break;
     
        tcp_send(connection_socket, msg);
        printf("Invio: %s\n", msg);
    }
    
    close(connection_socket);
    return 0;
}