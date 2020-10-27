// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Brief   -   Assignment 02 Part 5 source
//   Date    -   10-14-20
//
// ########################################## 

#include "../include/background.h"

//#define EXEC_BACK



int background_update_procs(ProcessList *list) {
    if(list->num_processes == 0) return 0;

    ProcessNode *tmp = list->HEAD;
    while(tmp != list->TAIL) {
        tmp = tmp->next;

        int exited = waitpid(tmp->prev->node->pid, NULL, WNOHANG);
        if(exited != 0) {
            process_list_rem_node(list, tmp->prev->node);
        }
    }
    
    int exited = waitpid(tmp->node->pid, NULL, WNOHANG);
    if(exited != 0) {
        process_list_rem_node(list, tmp->node);
    }
    
}
int background_place_proc(ProcessList *list, Process *proc) {

    proc->initialized = true;
    proc->returned = false;

    process_list_add_node(list, proc);
}



#ifdef EXEC_BACK
int main() {
    background_test();
}
#endif // EXEC_BACK