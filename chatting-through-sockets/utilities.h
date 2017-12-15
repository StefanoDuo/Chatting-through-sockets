#ifndef UTILITIES_H
#define UTILITIES_H



#include <stdbool.h>
#include <inttypes.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>



#define MAX(a, b) ((a) > (b) ? (a) : (b))



// Returns the conversion of str to uint16_t
// stops process execution if an error occurs during the conversion
uint16_t
get_port_number(const char *str);



#endif