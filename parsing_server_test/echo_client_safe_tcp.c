#include "safe_tcp.h"
#include "safe_socket.h"

#define BUFFER_SIZE 128

int main() {
    int port_number = 8080;
    char ip_address[] = "127.0.0.1";
    char msg[BUFFER_SIZE];

    int socket_des = create_tcp_socket();

    printf("Provo a connettermi con %s:%d\n", ip_address, port_number);
    safe_connect(socket_des, ip_address, port_number);
    printf("Connessione accettata\n");
    
    for(;;) {
        fgets(msg, BUFFER_SIZE, stdin);
        msg[strlen(msg) - 1] = '\0';    // removes trailing '\n'
      
        tcp_send(socket_des, msg);
        printf("Invio: %s", msg);
        
        if(strcmp(msg, "bye\n") == 0) {
            printf("Chiudo la connessione\n");
            break;
        }

        int result = tcp_receive(socket_des, msg, sizeof(msg));
        if (result == 0) {
            printf("La connessione e' stata chiusa\n");
            break;
        }
        printf("Ho ricevuto: %s\n", msg);
    }
    
    close(socket_des);
    return 0;
}