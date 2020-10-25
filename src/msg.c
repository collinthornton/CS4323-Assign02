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

#define SIZE 500

Msg* msg_allocate(char* cmd, char* ret, char *dir) {
    Msg *msg = (Msg*)malloc(sizeof(Msg));

    if(msg == NULL) {
        perror("msg: allocation");
        return NULL;                    // Return NULL if allocation fails
    }

    msg->cmd = (char*)calloc(SIZE, sizeof(char));
    msg->ret = (char*)calloc(SIZE, sizeof(char));
    msg->dir = (char*)calloc(SIZE, sizeof(char));
    msg->show_prompt = true;

    if(msg->cmd == NULL) {
        perror("msg->command: allocation");
        return NULL;
    }
    if(msg->ret == NULL) {
        perror("msg->ret: allocation");
        return NULL;
    }
    if(msg->dir == NULL) {
        perror("msg->dir: allocation");
        return(NULL);
    }

    if(cmd != NULL) {
        strcpy(msg->cmd, cmd);
    }
    if(ret != NULL) {
        strcpy(msg->ret, ret);
    }
    if(dir != NULL) {
        strcpy(msg->dir, dir);
    }

    return msg;
}

void msg_deallocate(Msg *msg) {
    if(msg != NULL) {
        if(msg->cmd != NULL) free(msg->cmd);
        if(msg->ret != NULL) free(msg->ret);
        if(msg->dir != NULL) free(msg->dir);

        free(msg);
        msg = NULL;
    }
}


char* msg_serialize(Msg *msg, char *buff) { 
    
    if(msg->cmd == NULL) return NULL;

    strcpy(buff, msg->cmd);

    if(msg->ret == NULL) {
        strcat(buff, ":-:null");
    } else {
        strcat(buff, ":-:");
        strcat(buff, msg->ret);
    }

    if(msg->dir == NULL) {
        strcat(buff, ":-:null");
    } else {
        strcat(buff, ":-:");
        strcat(buff, msg->dir);
    }

    strcat(buff, ":-:");
    if(msg->show_prompt) strcat(buff, "t");
    else strcat(buff, "f");

    strcat(buff, "\r\n");
    return buff;    
}


Msg* msg_deserialize(const char* str) {
    char *cmd = (char*)calloc(500, sizeof(char));
    char *ret = (char*)calloc(500, sizeof(char));
    char *dir = (char*)calloc(500, sizeof(char));
    char prmpt[1];

    char *pos1 = strstr(str, ":-:");
    strncpy(cmd, str, strlen(str)-strlen(pos1));

    char *pos2 = strstr(str+(strlen(str)-strlen(pos1)+3), ":-:");
    strncpy(ret, pos1+3, strlen(pos1)-strlen(pos2)-3);

    char *pos3 = strstr(str+(strlen(str)-strlen(pos2)+3), ":-:");
    strncpy(dir, pos2+3, strlen(pos2)-strlen(pos3)-3);

    char *pos4 = strstr(str+(strlen(str)-strlen(pos3)+3), "\r\n");
    strncpy(prmpt, pos3+3, strlen(pos3)-strlen(pos4)-3);

    Msg *msg = msg_allocate(cmd, ret, dir);
    msg->show_prompt = (strcmp(prmpt, "t") == 0) ? true : false;

    if(cmd != NULL) free(cmd);
    if(ret != NULL) free(ret);
    if(dir != NULL) free(dir);

    return msg;     
}