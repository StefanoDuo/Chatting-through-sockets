CC = gcc
CFLAGS = -Wall -std=c99
CLIENT_NAME = msg_client
SERVER_NAME = msg_server



all: client server



client: msg_client.o chat_app_client.o client_command_parser.o safe_tcp.o safe_udp.o safe_socket.o utilities.o
	$(CC) $(CFLAGS) msg_client.o chat_app_client.o client_command_parser.o safe_tcp.o safe_udp.o safe_socket.o utilities.o -o $(CLIENT_NAME)

server: msg_server.o chat_app_server.o server_command_parser.o safe_tcp.o safe_socket.o utilities.o
	$(CC) $(CFLAGS) msg_server.o chat_app_server.o server_command_parser.o safe_tcp.o safe_socket.o utilities.o -o $(SERVER_NAME)



msg_client.o: chat_app_client.h safe_udp.h safe_tcp.h safe_socket.h utilities.h chat_app_sizes.h

chat_app_client.o: chat_app_client.h client_command_parser.h safe_udp.h safe_tcp.h safe_socket.h chat_app_sizes.h commands_format.h

client_command_parser.o: client_command_parser.h commands_format.h

msg_server.o: chat_app_server.h safe_tcp.h safe_socket.h utilities.h chat_app_sizes.h

chat_app_server.o: chat_app_server.h server_command_parser.h safe_tcp.h safe_socket.h chat_app_sizes.h commands_format.h

server_command_parser.o: server_command_parser.h commands_format.h



safe_tcp.o: safe_tcp.h safe_socket.h

safe_udp.o: safe_udp.h safe_socket.h

safe_socket.o: safe_socket.h

utilities.o: utilities.h



clean:
	rm -f *.o $(SERVER_NAME) $(CLIENT_NAME)
