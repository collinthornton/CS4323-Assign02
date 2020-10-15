// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 02 process struct header
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
typedef struct {
    Process *node;
    Process *next;
    Process *prev;
} ProcessNode;


/*
// @brief Linked list of processes
*/
typedef struct {
    ProcessNode *HEAD;
    ProcessNode *TAIL;

    int num_processes;
} ProcessList;


Process* process_init(Process* proc, Msg *msg);
ProcessNode* process_node_init(ProcessNode *node, Process *proc, Process *next, Process *prev);
ProcessList* process_list_init(ProcessList *list, ProcessNode *HEAD, ProcessNode *TAIL);

int add_node(ProcessList *list, Process *proc);
int remove_node(ProcessList *list, Process *proc);

#endif // PROCESS_H