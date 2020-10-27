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


/**
 * @brief Initialize fields of Process struct
 * @param proc (Process*) process to be initialized
 * @param msg (Msg*) Msg from which to fill Process
 * @return (Process*) pointer to proc
 */
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


/**
 * @brief Deallocated all fields of Process struct
 * @param proc (Process*) process to be deallocated
 * @return (int) return code
 */
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


/**
 * @brief initalize process linked list
 * @param list (ProcessList*) Pointer to list to be initialized
 * @param HEAD (ProcessNode*) Pointer to HEAD of list. NULL if unused.
 * @param TAIL (ProcessNode*) Pointer to TAIL of list. NULL if unused.
 * @return (ProcessList*) same as list
 */
ProcessList* process_list_init(ProcessList *list, ProcessNode *HEAD, ProcessNode *TAIL) {
    list->num_processes = 0;

    if(HEAD != NULL) ++list->num_processes;
    if(TAIL != NULL && TAIL!= HEAD) ++list->num_processes;

    list->HEAD = HEAD;
    list->TAIL = TAIL;
}


/**
 * @brief add node to linked list
 * @param list (ProcessList*) pointer to list
 * @param proc (Process*) process to be added
 * @return (int) size of list
 */
int process_list_add_node(ProcessList *list, Process *proc) {
    if(list == NULL) return -1;
    if(list->num_processes < 0) return -2;
    
    Process *new_proc = (Process*)malloc(sizeof(Process));
    ProcessNode *new_node = (ProcessNode*)malloc(sizeof(ProcessNode));
    new_node->node = new_proc;
    new_node->next = NULL;
    new_node->prev = NULL;

    new_proc->initialized = proc->initialized;
    new_proc->pid = proc->pid;
    new_proc->returned = proc->returned;
    new_proc->num_args = proc->num_args;

    new_proc->exec = (char*)calloc(5000, sizeof(char));
    new_proc->ret = (char*)calloc(5000, sizeof(char));
    new_proc->args = (char**)malloc((proc->num_args+1)*sizeof(char*));
    for(int i=0; i<new_proc->num_args; ++i) new_proc->args[i] = (char*)calloc(5000, sizeof(char));
    new_proc->args[new_proc->num_args] = NULL;

    if(proc->exec != NULL) {
        strcpy(new_proc->exec, proc->exec);
    }
    if(proc->ret != NULL) {
        strcpy(new_proc->ret, proc->ret);
    }
    if(proc->args != NULL) {
        for(int i=0; i<new_proc->num_args; ++i) {
            strcpy(new_proc->args[i], proc->args[i]);
        }
    }

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
    return list->num_processes;
}


/**
 * @brief remove node from linked list
 * @param list (ProcessList*) pointer to list
 * @param proc (Process*) process to be removed
 * @return (int) length of list
 */
int process_list_rem_node(ProcessList *list, Process *proc) {
    if(list == NULL) return -1;
    if(list->HEAD == NULL || list->TAIL == NULL || list->num_processes <= 0) return -2;


    ProcessNode *tmp = list->HEAD;
    while(tmp != list->TAIL) {
        tmp = tmp->next;
    }

    while(tmp->node->pid != proc->pid) {
        if(tmp == list->TAIL) return -3;
        tmp = tmp->next;
    }

    if(list->num_processes == 1) {
        list->HEAD = NULL;
        list->TAIL = NULL;
        free(tmp->node->exec);
        free(tmp->node->ret);
        for(int i=0; i<tmp->node->num_args; ++i) free(tmp->node->args[i]);
        free(tmp->node->args);

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


/**
 * @brief Deallocate list, all nodes, and all process in list
 * @param list (ProcessList*) list to be removed
 * @return (int) return code
 */
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


/**
 * @brief convert process list to string
 * @param list (ProcessList*) list to be converted
 * @param buff (char[]) string buffer
 * @return (char*) pointer to buff
 */
const char* process_list_to_string(ProcessList *list, char buff[]) {
    sprintf(buff, "%d jobs\r\n", list->num_processes);
    
    if(list->HEAD == NULL || list->TAIL == NULL) return buff;
    
    sprintf(buff + strlen(buff), "CMD\t\t\tPID\r\n\r\n");

    ProcessNode *tmp = list->HEAD;
    sprintf(buff + strlen(buff), "0.) %s\t\t%d\r\n", tmp->node->exec, tmp->node->pid);
    
    int i=0;
    while(tmp != list->TAIL) {
        tmp = tmp->next;
        ++i;
        sprintf(buff + strlen(buff), "%d.) %s\t\t%d\r\n", i, tmp->node->exec, tmp->node->pid);
    }

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