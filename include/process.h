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


/**
// @brief Stores data for single processes. Allocated on heap
*/
typedef struct {
    bool initialized;           // Is process initialized
    bool returned;              // Has process returned

    int pid;
    int num_args;

    char *exec;
    char **args;
    char *ret;
} Process;


/**
// @brief Node in linked list of processes
*/
struct ProcessNode{
    Process *node;
    struct ProcessNode *next;
    struct ProcessNode *prev;
};
typedef struct ProcessNode ProcessNode;


/**
// @brief Linked list of processes
*/
typedef struct {
    ProcessNode *HEAD;
    ProcessNode *TAIL;

    int num_processes;
} ProcessList;


/**
 * @brief Initialize fields of Process struct
 * @param proc (Process*) process to be initialized
 * @param msg (Msg*) Msg from which to fill Process
 * @return (Process*) pointer to proc
 */
Process* process_init(Process* proc, Msg *msg);

/**
 * @brief Deallocated all fields of Process struct
 * @param proc (Process*) process to be deallocated
 * @return (int) return code
 */
int process_rem(Process* proc);

/**
 * @brief initalize process linked list
 * @param list (ProcessList*) Pointer to list to be initialized
 * @param HEAD (ProcessNode*) Pointer to HEAD of list. NULL if unused.
 * @param TAIL (ProcessNode*) Pointer to TAIL of list. NULL if unused.
 * @return (ProcessList*) same as list
 */
ProcessList* process_list_init(ProcessList *list, ProcessNode *HEAD, ProcessNode *TAIL);

/**
 * @brief add node to linked list
 * @param list (ProcessList*) pointer to list
 * @param proc (Process*) process to be added
 * @return (int) size of list
 */
int process_list_add_node(ProcessList *list, Process *proc);

/**
 * @brief remove node from linked list
 * @param list (ProcessList*) pointer to list
 * @param proc (Process*) process to be removed
 * @return (int) length of list
 */
int process_list_rem_node(ProcessList *list, Process *proc);

/**
 * @brief Deallocate list, all nodes, and all process in list
 * @param list (ProcessList*) list to be removed
 * @return (int) return code
 */
int process_list_del_list(ProcessList *list);


/**
 * @brief convert process list to string
 * @param list (ProcessList*) list to be converted
 * @param buff (char[]) string buffer
 * @return (char*) pointer to buff
 */
const char* process_list_to_string(ProcessList *list, char buff[]);

#endif // PROCESS_H