#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 2048

int main(int argc, char* argv[]) {
    int client_socket,
        result,
        port_number;
    struct sockaddr_in server_addr;
    char msg[BUFFER_SIZE] = "";
    
    uint16_t msg_length;
    size_t str_length;
    ssize_t ret;
    
    
    if(argc < 3) {
        printf("Specifica l'indirizzo ip e il numero di porta\n");
        return 1;
    }
    port_number = atoi(argv[2]);

    
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket == -1) {
        perror("Error during socket()");
        return 1;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);


    result = connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(result == -1) {
        perror("Error during connect()");
        return 1;
    }
    printf("Connessione accettata\n");
    
    
    for(;;) {
        fgets(msg, BUFFER_SIZE, stdin);
      
        if(strcmp(msg, "bye\n") == 0) {
            printf("Chiudo la connessione\n");
            break;
        }
      
        // invio la lunghezza del messaggio
        str_length = strlen(msg) + 1;
        msg_length = htons(str_length);
        ret = send(client_socket, (void *)&msg_length, sizeof(uint16_t), 0);
        if(ret == -1) {
            perror("Error during send()");
            return 1;
        }
        printf("Lunghezza messaggio: %zu\n", str_length);
      
        // invio il messaggio
        ret = send(client_socket, (void *)&msg, str_length, 0);
        if(ret == -1) {
            perror("Error during send()");
            return 1;
        }
        printf("Invio: %s\n", msg);
      
        ret = recv(client_socket, (void *)msg, str_length, 0);
        if(ret == -1) {
            perror("Error during recv()");
            return 1;
        }
        printf("Ho ricevuto: %s\n", msg);
    }
    
    
    close(client_socket);
    return 0;
}