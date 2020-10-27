// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Brief   -   Assignment 02 msg struct header
//   Date    -   10-14-20
//
// ########################################## 

#ifndef MSG_H
#define MSG_H

#include <stdbool.h>

#define MAX_CMD_SIZE 1024
#define MAX_RETURN_SIZE 1024

/**
 * @brief msg stuct used for comms on socket
 */
typedef struct {
    char *cmd;
    char *ret;

    char* dir;
    bool show_prompt;
} Msg;


/**
 * @brief allocate a Msg on heap. Must subsequently free memory with msg_deallocate()
 * @param cmd (char*) string command. set to NULL if unused
 * @param ret (char*) string return. set to NULL if unused
 * @param dir (char*) current working directory set to NULL if unused
 * @return (Msg*) Pointer to Msg allocated on heap
 */
Msg* msg_allocate(char* cmd, char *ret, char* dir);      \

/**
 * @brief Deallocate Msg previously allocated by msg_allocate()
 * @param msg (Msg*) msg allocated on heap
 */
void msg_deallocate(Msg *msg);                 \

/**
 * @brief Convert Msg to string for socket comms. Should be returned to Msg by msg_deserialize()
 * @param msg (Msg*) Msg to be converted
 * @param buff (char*) String buffer
 */
char* msg_serialize(Msg *msg, char *buff);              \

/**
 * @brief Conver string to Msg for socket comms. Should be preceded by msg_serialize()
 * @param str (const char*) String containing Msg information
 * @return (Msg*) msg struct allocated on heap
 */
Msg* msg_deserialize(const char* str);            

#endif //MSG_H
