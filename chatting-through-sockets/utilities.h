#ifndef UTILITIES_H
#define UTILITIES_H



#include <inttypes.h>



#define MAX(a, b) ((a) > (b) ? (a) : (b))



/* Returns the conversion of str to uint16_t
 *
 * Stops process execution if an error occurs during the conversion
 */
uint16_t
get_port_number(const char *str);



void
set_sigpipe_handler();



#endif
