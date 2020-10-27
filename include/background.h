// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Brief   -   Assignment 02 Part 5 include
//   Date    -   10-14-20
//
// ########################################## 


#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <stdio.h>		// for printf()
#include <unistd.h>		// for fork()
#include <stdlib.h>		// for exit()
#include <fcntl.h>
#include <string.h>

#include <sys/wait.h>   // for wait()

#include "../include/msg.h"
#include "../include/process.h"

/**
 * @brief Places a process into the background list
 * @param list (ProcessList*) Pointer to the list
 * @param proc (Process*) Pointer to the process to be added
 * @return (int) Length of list
 */
int background_place_proc(ProcessList *list, Process *proc);

/**
 * @brief Remove exited processes from list
 * @param list (ProcessList*) Pointer to the list
 * @return (int) Length of list
 */
int background_update_procs(ProcessList *list);

#endif // BACKGROUND_H