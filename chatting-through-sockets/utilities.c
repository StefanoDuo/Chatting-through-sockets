#include "utilities.h"

#include <stdio.h>
#include <unistd.h>



uint16_t
get_port_number(const char *str)
{
    uint16_t port_number;
    int items_matched = sscanf(str, "%" SCNu16, port_number);
    if (items_matched == 0) {
        printf("An error occurred while parsing <port_number>\n");
        exit(-1);
    }

    return port_number;
}