#ifndef COMMANDS_FORMAT_PARSER_H
#define COMMANDS_FORMAT_PARSER_H



/****************************************************
 ************ CLIENT --> SERVER REQUESTS ************
 ****************************************************/


/* C --> S "register_code;username;ip_address;port_number"
 *
 * S --> C "response_code;response_message"
 */
#define REGISTER 0




/* C --> S "who_code"
 * 
 * S --> C "number_of_registered_users"
 * for every registered user:
 *     S --> C "username(status)"
 */
#define WHO 1




/* C --> S "quit_code"
 *
 * No reponse because it can't fail
 */
#define QUIT 2




/* C --> S "deregister_code"
 *
 * No reponse because it can't fail
 */
#define DEREGISTER 3




/* C --> S "offline_send_code;receiving_username;offline_message"
 *
 * S --> C "response_message"
 *
 * We don't send a response code because whatever happens the client action is
 * the same, the client only need to show the response message to tell the user
 * if the offline send was successfull or not
 */
#define SEND 4




/* C --> S "resolve_name_code;username"
 *
 * S --> C "response_code;ip_address;port_number"
 *
 * ip_address and port_number are added only when the action didn't fail i.e.
 * when reponse code is different from USERNAME_NOT_FOUND and USERNAME_NOT_ONLINE
 */
#define RESOLVE_NAME 5	// Implicitly done by a client side !send




// Handled by the client side
#define HELP 6



/****************************************************
 ****************** RESPONSE CODES ******************
 ****************************************************/
// Those aren't really well defined
#define SUCCESS 1
#define UNKNOWN_COMMAND -1
#define ERROR -2
#define USERNAME_NOT_FOUND -1
#define USERNAME_NOT_ONLINE -2
#define USERNAME_ONLINE 1



// Delimiter used by the application protocol
#define DELIMITER ";"
#define C_DELIMITER ';'



#endif
