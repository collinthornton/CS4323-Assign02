// ##########################################
// 
//   Author  -   Group I
//   Email   -   collin.thornton@okstate.edu
//   Assign  -   Assignment 02 main
//   Date    -   10-14-20
//
// ########################################## 


#include "../include/msg.h"
//#include "../include/process.h"

#include <stdio.h>		// for printf()
#include <unistd.h>		// for fork()
#include <stdlib.h>		// for exit()

#include <sys/wait.h>   // for wait()

int server(void);
int client(void);



int main(int argc, char** argv) {

    pid_t pids[2];
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
    // SERVER INTERFACE



    // HISTORY



    // INTERPRETER



    // FOREGROUND



    // BACKGROUND

    return 0;
}



int client(void) {
    // SHELL INTERFACE




    // CLIENT INTERFACE

    return 0;
}