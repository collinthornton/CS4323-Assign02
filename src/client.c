#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define sockA struct sockaddr
#define SOCKET_BUFF 5000

#include "../include/client.h"
#include "../include/msg.h"

//#define EXEC_CLIENT

bool exit_flag = false;

pthread_t sock_tid, stdin_tid;
int sockfd;

char stdin_input[1024];
bool stdin_input_read = false;

char socket_input[SOCKET_BUFF];
int socket_queue_len = 0;

//sem_t sem;

int client(void) {
    #ifdef VERBOSE
    printf("|----- CLIENT:\t%d, %d\r\n", getppid(), getpid());
    #endif // VERBOSE


    const int PORT = 8081;

    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("client: socket()");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if(connect(sockfd, (sockA*)&servaddr, sizeof(servaddr)) != 0) {
        perror("client: connect()");
        exit(1);
    } 
    

    // SHELL INTERFACE
    char dir[1024];
    getcwd(dir, sizeof(dir));

    const char *user = getenv("USER");

    char home_dir[256];
    strcpy(home_dir, "/home/");
    strcat(home_dir, user);

    pthread_create(&stdin_tid, NULL, inputThread, NULL);
    pthread_create(&sock_tid, NULL, socketReadThread, NULL);

    (strcmp(dir, home_dir) == 0) ? printf("%s@CS4323shell:%s~$ ", user, dir) : printf("%s@CS4323shell:%s$ ", user, dir);


    while(1) {

        // HANDLE STDIN
        if(stdin_input_read == true) {
            stdin_input_read = false;

            // REMOVE NEW LINE CHARACTER
            stdin_input[strlen(stdin_input)-1] = '\0';
        
            // ALLOCATE MSG
            Msg *msg = msg_allocate(stdin_input, NULL, NULL);

            // SERIALIZE MSG
            char msg_buff[1024];
            bzero(msg_buff, 1024);
            msg_serialize(msg, msg_buff);
            
            // WRITE TO SOCKET
            write(sockfd, msg_buff, SOCKET_BUFF);

            // DEALLOCATE MSG
            msg_deallocate(msg);

            // QUIT IF q RECEIVED
            if(strcmp(stdin_input, "q") == 0) break;
        }

        // HANDLE SOCKET
        if(socket_queue_len > 0) {
            --socket_queue_len;

            //printf("%s\r\n", socket_input);

            // DESERIALZE MESSAGE
            Msg *msg = msg_deserialize(socket_input);
            
            printf("%s\r\n", msg->ret);
            
            // SHOW PROMPT IF DESIRED
            if(msg->show_prompt) {
                if(strcmp(msg->dir, home_dir) == 0) {
                    printf("%s@CS4323shell:%s~$ ", user, msg->dir);
                } else {
                    printf("%s@CS4323shell:%s$ ", user, msg->dir);
                } 
                fflush(stdout);
            }

            // DEALLOCATE MESSAGE
            msg_deallocate(msg);
        }


        // Sleep for 0.01 second
        struct timespec ts;
        ts.tv_sec  = 1E-2;
        ts.tv_nsec = 1E7;
        nanosleep(&ts, &ts);
    }

    close(sockfd);
    exit_flag = true;
    pthread_join(stdin_tid, NULL);
    pthread_join(sock_tid, NULL);
    printf("Goodbye\r\n");

    return 0;
}


// THREAD TO READ STDIN
void *inputThread(void *vargp) {
    while(!exit_flag) {
        fgets(stdin_input, sizeof(stdin_input), stdin);
        stdin_input_read = true;
           
        struct timespec ts;
        ts.tv_sec  = 1E-2;
        ts.tv_nsec = 1E7;
        nanosleep(&ts, &ts);
    }
}

// THREAD TO READ SOCKET
void *socketReadThread(void *vargp) {
    while(!exit_flag) {
        char buff[SOCKET_BUFF];
        bzero(buff, SOCKET_BUFF);

        if(read(sockfd, buff, sizeof(buff)) == -1) {
            perror("client:  socket closed");
            exit(1);
        }
        strcpy(socket_input, buff);
        ++socket_queue_len;
    }
}


#ifdef EXEC_CLIENT
int main(int argc, char** argv) {
    client();
    return 0;
}
#endif // EXEC_CLIENT