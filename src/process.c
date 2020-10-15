// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 02 process struct source
//   Date    -   10-14-20
//
// ########################################## 

#include "../include/process.h"


Process* process_init(Process *proc, Msg *msg) {
    proc->initialized = false;
    proc->returned = false;
    proc->pid = -1;
    proc->msg = msg;
}

ProcessNode* process_node_init(ProcessNode *node, Process *proc, Process *next, Process *prev) {
    node->node = proc;
    node->next = next;
    node->prev = prev;
}

ProcessList* process_list_init(ProcessList *list, ProcessNode *HEAD, ProcessNode *TAIL) {
    list->num_processes = 0;

    if(HEAD != NULL) ++list->num_processes;
    if(TAIL != NULL && TAIL!= HEAD) ++list->num_processes;

    list->HEAD = HEAD;
    list->TAIL = TAIL;
}

int add_node(ProcessList *list, ProcessNode)