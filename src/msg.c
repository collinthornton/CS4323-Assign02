// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 02 msg struct source
//   Date    -   10-14-20
//
// ########################################## 

#include "../include/msg.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Msg* msg_allocate(char* cmd, char* ret, char *dir) {
    Msg *msg = (Msg*)malloc(sizeof(Msg));

    if(msg == NULL) {
        perror("msg: allocation");
        return NULL;                    // Return NULL if allocation fails
    }

    msg->cmd = NULL;
    msg->ret = NULL;
    msg->dir = NULL;

    if(cmd != NULL) {
        int size_cmd = sizeof(cmd);
        msg->cmd = (char*)malloc(size_cmd + 1);
        if(msg->cmd == NULL) {
            perror("msg->command: allocation");
            return NULL;
        }
        strcpy(msg->cmd, cmd);
    }

    if(ret != NULL) {
        int size_ret = sizeof(ret);
        msg->ret = (char*)malloc(size_ret + 1);

        if(msg->ret == NULL) {
            perror("msg->ret: allocation");
            return NULL;
        }
        strcpy(msg->ret, ret);
    }

    if(dir != NULL) {
        int size_cmd = sizeof(dir);
        msg->dir = (char*)malloc(size_cmd + 1);

        if(msg->dir == NULL) {
            perror("msg->dir: allocation");
            return(NULL);
        }
        strcpy(msg->dir, dir);
    }

    return msg;
}

void msg_deallocate(Msg *msg) {
    if(msg != NULL) {
        if(msg->cmd != NULL) {
            free(msg->cmd);
        }

        if(msg->ret != NULL) {
            free(msg->ret);
        }

        free(msg);
    }
}


// const char* msg_serialize(Msg *msg) { DEFINE ME HERE }
// Msg* msg_deserialize(const char* str) { DEFINE ME HERE }