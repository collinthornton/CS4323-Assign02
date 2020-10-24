#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#include "../include/client.h"

#define EXEC_CLIENT


char input[1024];
bool input_read = false;

//sem_t sem;

int client(void) {
    #ifdef VERBOSE
    printf("|----- CLIENT:\t%d, %d\r\n", getppid(), getpid());
    #endif // VERBOSE

    // SHELL INTERFACE
    char dir[1024];
    getcwd(dir, sizeof(dir));

    const char *user = getenv("USER");

    char home_dir[256];
    strcpy(home_dir, "/home/");
    strcat(home_dir, user);

    pthread_t tid;
    pthread_create(&tid, NULL, inputThread, NULL);

    (strcmp(dir, home_dir) == 0) ? printf("%s@CS4323shell:%s~$ ", user, dir) : printf("%s@CS4323shell:%s$ ", user, dir);


    while(1) {
        Msg *msg;
        // PRINT PROMPT IF INPUT READ
        if(input_read == true) {
            input_read = false;

            // REMOVE NEW LINE CHARACTER
            input[strlen(input)-1] = '\0';
        
            // ALLOCATE MSG
            msg = msg_allocate(input, NULL, NULL);

            if(strcmp(input, "q") == 0) break;
            
            chdir(input);
            getcwd(dir, sizeof(dir));


            // PRINT PROMPT
            (strcmp(dir, home_dir) == 0) ? printf("%s@CS4323shell:%s~$ ", user, dir) : printf("%s@CS4323shell:%s$ ", user, dir);

            msg_deallocate(msg);  //! ERASE ME
        }

        // SEND TO CLIENT INTERFACE

        // CLEAR CLIENT INTERFACE BUFFER


        // Sleep for 0.01 second
        struct timespec ts;
        ts.tv_sec  = 1E-2;
        ts.tv_nsec = 1E7;
        nanosleep(&ts, &ts);
    }

    printf("Goodbye\r\n");
    pthread_cancel(tid);



    // CLIENT INTERFACE

    return 0;
}



void *inputThread(void *vargp) {
    while(1) {
        fgets(input, sizeof(input), stdin);
        input_read = true;
           
        struct timespec ts;
        ts.tv_sec  = 1E-2;
        ts.tv_nsec = 1E7;
        nanosleep(&ts, &ts);
    }
}



#ifdef EXEC_CLIENT
int main(int argc, char** argv) {
    client();
    return 0;
}
#endif // EXEC_CLIENT