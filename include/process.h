// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Brief   -   Assignment 02 process struct header
//   Date    -   10-14-20
//
// ########################################## 

#ifndef PROCESS_H
#define PROCESS_H

#include "msg.h"
#include <stdbool.h>


/*
// @brief Stores data for single processes. Allocated on heap
*/
typedef struct {
    bool initialized;           // Is process initialized
    bool returned;              // Has process returned

    int pid;

    Msg *msg;
} Process;


/*
// @brief Node in linked list of processes
*/
struct ProcessNode{
    Process *node;
    struct ProcessNode *next;
    struct ProcessNode *prev;
};
typedef struct ProcessNode ProcessNode;


/*
// @brief Linked list of processes
*/
typedef struct {
    ProcessNode *HEAD;
    ProcessNode *TAIL;

    int num_processes;
} ProcessList;


Process* process_init(Process* proc, Msg *msg);
ProcessList* process_list_init(ProcessList *list, ProcessNode *HEAD, ProcessNode *TAIL);

int process_list_add_node(ProcessList *list, Process *proc);
int process_list_rem_node(ProcessList *list, Process *proc);
int process_list_del_list(ProcessList *list);

const char* process_list_to_string(ProcessList *list, char buff[]);

#endif // PROCESS_H