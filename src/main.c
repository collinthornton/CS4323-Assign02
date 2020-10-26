// ##########################################
// 
//   Author  -   Group I
//   Email   -   collin.thornton@okstate.edu
//   Brief   -   Assignment 02 main
//   Date    -   10-14-20
//
// ########################################## 

#define VERBOSE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#include "../include/client.h"
#include "../include/server.h"

int server(void);
int client(void);

void test(void);

//#define EXEC_MAIN_TEST


#ifdef EXEC_MAIN_TEST
#include <string.h>
#endif // EXEC_MAIN_TEST


int main(int argc, char** argv) {
    #ifdef EXEC_MAIN_TEST
        test();
        return 0;
    #endif // EXEC_MAIN_TEST

    pid_t pids[2] = { 0, 0 };
    for(int i=0; i<2; ++i) {
        if((pids[i] = fork()) < 0) {        // SPAWN PROCESSES
            perror("Fork failed");
        }

        if(pids[0] == 0) {
            int server_ret = server();      // EXECUTE SERVER PROCESS
            //execl("sock", "sock", NULL);
            exit(server_ret);
        }
        if(pids[1] == 0) {
            sleep(1);
            int client_ret = client();      // EXECUTE CLIENT PROCESS
            exit(client_ret);
        }
    }

    for(int i=0; i<2; ++i) wait(NULL);    // WAIT FOR BOTH TO EXIT

    exit(0);                                // EXIT
}




#ifdef EXEC_MAIN_TEST
void test(void) {
    char test1[100] = "this is";
    char test2[100] = "this | is";

    printf("\r\ntmp1\r\n");
    char *tmp1 = strtok(test1, "|");
    do {
        printf("%s\r\n", tmp1);
        tmp1 = strtok(NULL, "|");
    } while(tmp1 != NULL);

    printf("\r\ntmp2\r\n");
    char *tmp2 = strtok(test2, "|");
    do {
        printf("%s\r\n", tmp2);
        tmp2 = strtok(NULL, "|");
    } while(tmp2 != NULL);
}
#endif // EXEC_MAIN_TEST