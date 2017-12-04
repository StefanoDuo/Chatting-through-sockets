#include "utilities.h"

bool
str_to_uint16(const char *str, uint16_t *res)
{
    char *end;
    errno = 0;
    intmax_t val = strtoimax(str, &end, 10);
    if (errno==ERANGE || val<0 || val>UINT16_MAX || end==str || *end!='\0')
        return false;
    *res = (uint16_t) val;
    return true;
}


uint16_t
get_port_number(const char *str)
{
    uint16_t port_number;
    if(!str_to_uint16(str, &port_number)) {
        printf("An error occurred while parsing <port_number>\n");
        exit(-1);
    }

    return port_number;
}