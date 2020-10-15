// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 02 msg struct header
//   Date    -   10-14-20
//
// ########################################## 

#ifndef MSG_H
#define MSG_H

#define MAX_CMD_SIZE 1024
#define MAX_RETURN_SIZE 1024


typedef struct {
    const char *cmd;
    char *ret;
} Msg;


Msg* msg_allocate(const char* cmd, char *ret);      // Allocate & initialize Msg struct on heap
void msg_deallocate(Msg *msg);                      // Deallocate Msg struct

const char* msg_serialize(Msg *msg);                // Convert Msg to string for socket
Msg* msg_deserialize(const char* str);              // Allocated Msg on heap and convert string to Msg

#endif //MSG_H
