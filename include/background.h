// ##########################################
// 
//   Author  -   Group I
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
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "../include/msg.h"
#include "../include/process.h"


int background_place_proc(ProcessList *list, Process *proc);
int background_update_procs(ProcessList *list);

const char* background_get_return();


#endif // BACKGROUND_H