#include "utilities.h"

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>



uint16_t
get_port_number(const char *str)
{
    uint16_t port_number;
    int items_matched = sscanf(str, "%" SCNu16, &port_number);
    if (items_matched == 0) {
        printf("An error occurred while parsing <port_number>\n");
        exit(-1);
    }

    return port_number;
}



static void
sigpipe_handler(int signum)
{
	printf("Error during send(): the other end closed the connection, received a SIGPIPE signal\n");
	exit(-1);
}


/* We need to set a SIGPIPE handler so we know if the other side of the connection
 * has closed its socket (SIGPIPE is received after a send() on a closed connection)
 * https://stackoverflow.com/questions/15406097/writing-on-a-tcp-socket-closed-by-the-peer
 */
void
set_sigpipe_handler()
{
	signal(SIGPIPE, &sigpipe_handler);
}
