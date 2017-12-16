#ifndef CHAT_APP_SIZES_H
#define CHAT_APP_SIZES_H



// TODO: define here every size used by the application
#define MAX_MESSAGE_LENGTH 256
#define MAX_USERNAME_LENGTH 32



#define MAX_COMMAND_LENGTH 5
#define MAX_PORT_NUMBER_LENGTH 5
#define DELIMITER_LENGTH 1


// "command;username;offline_message" where offline_message = "username>message"
#define MAX_BUFFER_SIZE (MAX_COMMAND_LENGTH) + (MAX_USERNAME_LENGTH) \
	+ (MAX_MESSAGE_LENGTH) + (DELIMITER_LENGTH) * 3 



#endif
