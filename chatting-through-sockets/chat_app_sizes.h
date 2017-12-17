#ifndef CHAT_APP_SIZES_H
#define CHAT_APP_SIZES_H


// Used client side to synch sizes with the server
#define MAX_MESSAGE_LENGTH 256
#define MAX_USERNAME_LENGTH 32



// Length of a string needed to represent every int16_t value
#define MAX_COMMAND_LENGTH 5
// Length of a string needed to represent every uint16_t value
#define MAX_PORT_NUMBER_LENGTH 5
// Length of the string used as a delimiter in our application protocol
#define DELIMITER_LENGTH 1
// Length of " (offline msg)"
#define OFFLINE_TAG_LENGTH 14



/* Length of the longest application protocol message, used to define a buffer
 * which can hold every message format without overflowing, our current longest
 * command is the one used to send offline messages:
 *
 * "command_id;receiver_username;offline_message"
 * where offline_message := "sender_username (offline msg)>\n message"
 */
#define MAX_BUFFER_SIZE (MAX_COMMAND_LENGTH) + (MAX_USERNAME_LENGTH) * 2 \
					  + (OFFLINE_TAG_LENGTH) + (MAX_MESSAGE_LENGTH) \
					  + (DELIMITER_LENGTH) * 5



#endif
