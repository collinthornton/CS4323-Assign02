// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Brief   -   Assignment 02 process struct source
//   Date    -   10-14-20
//
// ########################################## 

#include "../include/process.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//#define EXEC_PROCESS

Process* process_init(Process *proc, Msg *msg) {
    proc->initialized = false;
    proc->returned = false;
    proc->pid = -1;
    proc->ret = (char*)calloc(5000, sizeof(char));
    proc->args = NULL;
    proc->exec = (char*)calloc(1000, sizeof(char));
    proc->num_args = 0;

    char buff[MAX_CMD_SIZE];
    strcpy(buff, msg->cmd);
    char *token = strtok(buff, " ");
    if(token == NULL) {
        strcpy(proc->exec, msg->cmd);
        proc->args = (char**)malloc(sizeof(char*));
        proc->args[0] = NULL;
        proc->num_args = 0;
    }
    else {
        int i = 0;

        do {
            token = strtok(NULL, " ");
            ++i;
        } while(token != NULL);
        proc->num_args = i;

        proc->args = (char**)malloc((proc->num_args+1)*sizeof(char*));

        strcpy(buff, msg->cmd);
        token = strtok(buff, " ");
        proc->exec = (char*)malloc(sizeof(char[50]));
        strcpy(proc->exec, token);
        
        int j=0;
        proc->args[j] = (char*)malloc(sizeof(char[50]));
        strcpy(proc->args[j++], token);

        do {
            token = strtok(NULL, " ");

            if(token != NULL) {
                proc->args[j] = (char*)calloc(50, sizeof(char));
                strcpy(proc->args[j++], token);
            }
        } while(token != NULL);


       proc->args[j] = NULL;
    }

    return proc;
}

int process_rem(Process *proc) {
    if(proc->ret != NULL) free(proc->ret);
    if(proc->exec != NULL)free(proc->exec);

    if(proc->args != NULL) {
        for(int i=0; i<=proc->num_args; ++i) {
            free(proc->args[i]);
        }
        free(proc->args);
    }
}

ProcessList* process_list_init(ProcessList *list, ProcessNode *HEAD, ProcessNode *TAIL) {
    list->num_processes = 0;

    if(HEAD != NULL) ++list->num_processes;
    if(TAIL != NULL && TAIL!= HEAD) ++list->num_processes;

    list->HEAD = HEAD;
    list->TAIL = TAIL;
}

int process_list_add_node(ProcessList *list, Process *proc) {
    if(list == NULL) return -1;
    if(list->num_processes < 0) return -2;
    
    Process *new_proc = (Process*)malloc(sizeof(Process));
    ProcessNode *new_node = (ProcessNode*)malloc(sizeof(ProcessNode));
    new_node->node = new_proc;
    //new_node->node->msg = proc->msg;
    new_node->node->initialized = proc->initialized;
    new_node->node->pid = proc->pid;
    new_node->node->returned = proc->returned;

    if(list->num_processes == 0) {
        list->HEAD = new_node;
        list->TAIL = new_node;
    }
    else {
        list->TAIL->next = new_node;
        new_node->prev = list->TAIL;
        list->TAIL = new_node;
    }

    ++list->num_processes;
    printf("PROCESS ADD NODE INCREMENTING %d\r\n", list->num_processes);

    return list->num_processes;
}

int process_list_rem_node(ProcessList *list, Process *proc) {
    if(list == NULL) return -1;
    if(list->HEAD == NULL || list->TAIL == NULL || list->num_processes <= 0) return -2;

    printf("PROCESS LIST REM NODE %d\r\n", proc->pid);
    printf("PROCESS LIST NUM NODES %d\r\n", list->num_processes);
    printf("TAIL: %p\r\n", list->TAIL);
    printf("HEAD: %p\r\n", list->HEAD);

    ProcessNode *tmp = list->HEAD;

    printf("%p\r\n", tmp->node);
    while(tmp != list->TAIL) {
        tmp = tmp->next;
        printf("%p\r\n", tmp->node);
    }

    while(tmp->node->pid != proc->pid) {
        if(tmp == list->TAIL) return -3;
        tmp = tmp->next;
    }

    if(list->num_processes == 1) {
        list->HEAD = NULL;
        list->TAIL = NULL;
        free(tmp->node);
        free(tmp);
        --list->num_processes;
        return list->num_processes;
    }

    if(tmp == list->HEAD) {
        list->HEAD = tmp->next;
        list->HEAD->prev = NULL;
    }
    else if(tmp == list->TAIL) {
        list->TAIL = tmp->prev;
        list->TAIL->next = NULL;
    }
    else {
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
    }

    --list->num_processes;
    free(tmp->node);
    free(tmp);
    return list->num_processes;
}

int process_list_del_list(ProcessList *list) {
    if(list == NULL) return -1;
    if(list->HEAD == NULL || list->TAIL == NULL || list->num_processes < 0) return -2;

    ProcessNode *tmp = list->HEAD;
    while(tmp != list->TAIL) {
        tmp = tmp->next;
        free(tmp->prev->node);
        free(tmp->prev);
        --list->num_processes;
    }
    free(tmp->node);
    free(tmp);
    --list->num_processes;
    return list->num_processes;
}

const char* process_list_to_string(ProcessList *list, char buff[]) {
    if(list->HEAD == NULL || list->TAIL == NULL) return NULL;
    
    ProcessNode *tmp = list->HEAD;
    sprintf(buff, "%d\n", tmp->node->pid);//s\r\n", tmp->node->pid, tmp->node->exec);
    do {
        tmp = tmp->next;
        sprintf(buff + strlen(buff), "%d\r\n", tmp->node->pid);//\t%s\r\n", tmp->node->pid, tmp->node->exec);
    } while(tmp != list->TAIL);

    return buff;
}



#ifdef EXEC_PROCESS
int main() {
    Msg msg;
    msg.cmd = "ps -A";
    msg.ret = NULL;

    Process proc;
    process_init(&proc, &msg);

    printf("%s\r\n", proc.exec);

    for(int i=0; i<proc.num_args+1; ++i) printf("%s\r\n", proc.args[i]);

    process_rem(&proc);
}
#endif // EXEC_PROCESS