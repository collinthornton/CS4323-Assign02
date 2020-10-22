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

/*
int* background_init(int* shd_ptr) {
    int shm_fd = shm_open(BG_SHMEM, O_CREAT | O_RDWR, 0666);

    const int SHM_SIZE = sizeof(int[100]);
    ftruncate(shm_fd, SHM_SIZE);

    shd_ptr = (int*)mmap(0, SHM_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    return shd_ptr;
}
*/


int background_update_procs(ProcessList *list) {
    printf("BACKGROUND UPDATE PROCS %d\r\n", list->num_processes);
    if(list->num_processes == 0) return 0;

    ProcessNode *tmp = list->HEAD;
    do {
        int exited = waitpid(tmp->node->pid, NULL, WNOHANG);
        
        if(exited != 0) {
            printf("DELETING %d\r\n", tmp->node->pid);
            process_list_rem_node(list, tmp->node);
        }

    } while(tmp != list->TAIL);
    
}
int background_place_proc(ProcessList *list, Process *proc) {
    pid_t pid;
    int newfd = 0;
    /*
    char exec[50];
    
    strcpy(proc->exec, exec);

    char args[proc->num_args+1][50];
    for(int i=0; i<proc->num_args; ++i) {
        strcpy(proc->args[i], args[i]);
    }
    proc->args[proc->num_args] = NULL;*/

    if((proc->pid = fork()) == 0) {
        printf("Background child spawned: %d\r\n", getpid());

        //printf("%s ", proc->exec);
        //for(int i=0; i<proc->num_args+1; ++i) printf("%s ", proc->args[i]);
        //printf("\r\n");

        //proc->pid = getpid();
        //proc->initialized = true;

        //printf("BACKGROUND PLACE PROC ADDING\r\n");
        //process_list_add_node(list, proc);

        //newfd = open("test.txt", O_CREAT|O_TRUNC|O_WRONLY, 0644);
        //dup2(newfd, 1);

        execvp(proc->exec, proc->args);

    }  else if(proc->pid < 0) perror("background_place_proc(): fork()");

    proc->initialized = true;
    printf("BACKGROUND PLACE PROC ADDING \r\n");
    process_list_add_node(list, proc);
    //sleep(1);

    int status;
   // while(waitpid(pid, &status, WNOHANG) == 0) { 
        #ifdef VERBOSE
        printf("PID %d executing\r\n", pid); 
        sleep(1); 
        #endif // VERBOSE
    //}

    #ifdef VERBOSE
    printf("\r\n\r\nCOMPLETE: %d\r\n", pid);

    if(WIFEXITED(status)) printf("Exited normally\r\n");
    else printf("Exited with error\r\n");
    #endif // VERBOSE

    //if(newfd > 0) close(newfd);
}



void background_test() {
    Msg msg1;
    msg1.cmd = "find /tmp";
    msg1.ret = NULL;

    Msg msg2;
    msg2.cmd = "psh -A";
    msg2.ret = NULL;

    Process proc1;
    process_init(&proc1, &msg1);

    Process proc2;
    process_init(&proc2, &msg2);
    
    ProcessList list;
    process_list_init(&list, NULL, NULL);


    background_place_proc(&list, &proc1);
    background_place_proc(&list, &proc2);

    sleep(1);
    //background_update_procs(&list);
    printf("num processes: %d\r\n", list.num_processes);

   // while(list.num_processes > 0) {
    //    background_update_procs(&list);

        char buff[1024];
        process_list_to_string(&list, buff);
        printf("%s\r\n\r\n", buff);
        //sleep(1);
  //  }

    //process_list_del_list(&list);    
}


void process_list_test() {
    #ifdef VERBOSE
    printf("|----- BEGINNING PROCESS LIST TEST\r\n\r\n");
    #endif // VERBOSE

    Msg msg;
    msg.cmd = "/bin/ls";

    Process proc;
    process_init(&proc, &msg);
    proc.pid = getpid() + 1;

    Process proc1;
    process_init(&proc1, &msg);
    proc1.pid = getpid() + 2;

    Process proc2;
    process_init(&proc2, &msg);
    proc2.pid = getpid() + 3;

    ProcessList proc_list;
    process_list_init(&proc_list, NULL, NULL);

    process_list_add_node(&proc_list, &proc);
    process_list_add_node(&proc_list, &proc1);
    process_list_add_node(&proc_list, &proc2);

    ProcessNode *tmp = proc_list.HEAD;

    char buff[1024];

    printf("%s\r\n", process_list_to_string(&proc_list, buff));

    process_list_rem_node(&proc_list, &proc1);

    buff[0] = '\0';
    printf("%s\r\n", process_list_to_string(&proc_list, buff));

    process_list_del_list(&proc_list);
}



#ifdef EXEC_BACK
int main() {
    background_test();
}
#endif // EXEC_BACK