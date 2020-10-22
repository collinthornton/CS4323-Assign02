// ##########################################
// 
//   Author  -   Group I
//   Email   -   collin.thornton@okstate.edu
//   Brief   -   Assignment 02 main
//   Date    -   10-14-20
//
// ########################################## 

#define VERBOSE


#include "../include/background.h"

int server(void);
int client(void);


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


    //system("echo 'THIS IS A TEST WITH system()'");

    //sleep(3);

    // HISTORY



    // INTERPRETER



    // FOREGROUND



    // BACKGROUND
    //process_list_test();

    //background_test();

    //sleep(2);
    printf("Exiting server\r\n");

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




