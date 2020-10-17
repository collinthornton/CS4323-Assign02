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

Process* process_init(Process *proc, Msg *msg) {
    proc->initialized = false;
    proc->returned = false;
    proc->pid = -1;
    proc->msg = msg;
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
    new_node->node->msg = proc->msg;
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

    return list->num_processes;
}

int process_list_rem_node(ProcessList *list, Process *proc) {
    if(list == NULL) return -1;
    if(list->HEAD == NULL || list->TAIL == NULL || list->num_processes <= 0) return -2;

    ProcessNode *tmp = list->HEAD;
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
    sprintf(buff, "%d\t%s\r\n", tmp->node->pid, tmp->node->msg->cmd);
    do {
        tmp = tmp->next;
        sprintf(buff + strlen(buff), "%d\t%s\r\n", tmp->node->pid, tmp->node->msg->cmd);
    } while(tmp != list->TAIL);

    return buff;
}