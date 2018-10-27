# chatting-through-sockets
Command line chat application implemented using Posix sockets. Implemented using an hybrid architecture:
* client-server to manage registered and currently online users,
* peer-to-peer to send a receive messages

## Compilation instructions
Simply execute `make` while inside the `chatting-through-sockets` folder (requires a C99 compiler).

## How to chat
* To launch a chat application server simply execute `./msg_server port_number'
* To launch a chat application client instead `./msg_client local_ip local_port server_ip server_port`.

`local_ip` and `local_port` will be bound to the UDP socket used to send and receive messages with the other clients.
To execute locally simply use `127.0.0.1` for both `local_ip` and `server_ip`. Clients/servers behind a router using NAT are not supported.

### Client commands
* `!help` --> shows available commands
* `!register` <username> --> register client as <username>
* `!deregister` --> deregister client
* `!who` --> shows registered clients
* `!send <username>` --> sends a message to another registered client, a line containing only '.' ends the message
* `!quit` --> sets the username offline and exits

A message can be sent to an offline user, it will be buffered temporarely by the server and sent once the user logs in.
