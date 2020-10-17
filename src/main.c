// ##########################################
// 
//   Author  -   Group I
//   Email   -   collin.thornton@okstate.edu
//   Brief   -   Assignment 02 main
//   Date    -   10-14-20
//
// ########################################## 

#define VERBOSE

#include "../include/msg.h"
#include "../include/process.h"

#include <stdio.h>		// for printf()
#include <unistd.h>		// for fork()
#include <stdlib.h>		// for exit()

#include <sys/wait.h>   // for wait()

int server(void);
int client(void);


void process_list_test(void);


int main(int argc, char** argv) {

    pid_t pids[2] = { 0, 0 };
    for(int i=0; i<2; ++i) {
        if((pids[i] = fork()) < 0) {        // SPAWN PROCESSES
            perror("Fork failed");
        }

        if(pids[0] == 0) {
            int server_ret = server();      // EXECUTE SERVER PROCESS
            exit(server_ret);
        }
        if(pids[1] == 0) {
            int client_ret = client();      // EXECUTE CLIENT PROCESS
            exit(client_ret);
        }
    }

    for(int i=0; i<2; ++i) wait(NULL);    // WAIT FOR BOTH TO EXIT

    exit(0);                                // EXIT
}



int server(void) {
    #ifdef VERBOSE
    printf("|----- SERVER:\t%d, %d\r\n", getppid(), getpid());
    #endif // VERBOSE

    // SERVER INTERFACE


    // HISTORY



    // INTERPRETER



    // FOREGROUND



    // BACKGROUND
    process_list_test();

    return 0;
}



int client(void) {
    #ifdef VERBOSE
    printf("|----- CLIENT:\t%d, %d\r\n", getppid(), getpid());
    #endif // VERBOSE

    // SHELL INTERFACE



    // CLIENT INTERFACE

    return 0;
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