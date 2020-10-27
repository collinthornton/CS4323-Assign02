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

bool exit_flag = false;                 // FLAG TO CONTINUE EXECUTING

pthread_t sock_tid, stdin_tid;          // THREAD IDs
int sockfd;                             // FILE DESCRIPTOR FOR SOCKET

char stdin_input[1024];                 // BUFFER FOR stdin 
bool stdin_input_read = false;          // FLAG FOR NEW MESSAGE ON stdin

char socket_input[SOCKET_BUFF];         // BUFFER FOR SOCKET
int socket_queue_len = 0;               // FLAG FOR NEW MESSAGE ON socket


/**
 * @brief Run the client process
 * @return (int) return code
 */
int client(void) {

    // BEGIN SETUP
    
    #ifdef VERBOSE
    printf("|----- CLIENT:\t%d, %d\r\n", getppid(), getpid());
    #endif // VERBOSE

    bzero(socket_input, sizeof(socket_input));
    bzero(stdin_input, sizeof(stdin_input));
    
    // SETUP SOCKET
    const int PORT = 8081;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("client: socket()");
        exit(1);
    }

    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    if(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0) {
        perror("client: setsockopt");
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
    

    // SETUP SHELL INTERFACE

    const char *user = getenv("USER");

    char home_dir[256];
    strcpy(home_dir, "/home/");
    strcat(home_dir, user);


    // LAUNCH THREADS FOR stdin AND socket

    pthread_create(&stdin_tid, NULL, inputThread, NULL);
    pthread_create(&sock_tid, NULL, socketReadThread, NULL);


    // DISPLAY ASSIGNMENT INFORMATION

    printf("CS4323 ASSIGNMENT II GROUP I\r\n");
    printf("C SHELL EMULATOR\r\n\r\n");
    printf("Collin Thornton\r\nEthan Vascellaro\r\nKazi Sharif\r\nCaleb Goodart\r\n\r\n");
    printf("HELP DISPLAY:\r\n\r\n");


    // SEND INITIAL COMMAND TO DISPLAY help
    
    Msg *init_msg;
    init_msg = msg_allocate("help", NULL, NULL);
    char init_msg_buff[SOCKET_BUFF];
    bzero(init_msg_buff, sizeof(init_msg_buff));
    msg_serialize(init_msg, init_msg_buff);

    msg_deallocate(init_msg);

    if(write(sockfd, init_msg_buff, SOCKET_BUFF) < 0) {
        perror("client: write()");
        exit(1);
    }


    while(1) {

        // HANDLE STDIN
        if(stdin_input_read == true) {
            stdin_input_read = false;

            // REMOVE NEW LINE CHARACTER
            if(stdin_input[strlen(stdin_input)-1] == '\n') stdin_input[strlen(stdin_input)-1] = '\0';
        
            // ALLOCATE MSG
            Msg *msg = msg_allocate(stdin_input, NULL, NULL);

            // SERIALIZE MSG
            char msg_buff[SOCKET_BUFF];
            bzero(msg_buff, SOCKET_BUFF);
            msg_serialize(msg, msg_buff);
            
            // WRITE TO SOCKET
            if(write(sockfd, msg_buff, SOCKET_BUFF) < 0) {
                perror("client: write()");
                exit(1);
            }

            // DEALLOCATE MSG
            msg_deallocate(msg);

            // QUIT IF q RECEIVED
            if(strcmp(stdin_input, "exit") == 0) break;
        }

        // HANDLE SOCKET
        if(socket_queue_len > 0) {
            --socket_queue_len;

            // DESERIALZE MESSAGE
            Msg *msg = msg_deserialize(socket_input);
            printf("%s", msg->ret);
            fflush(stdout);
            
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



/**
 * @brief Manage input on stdin. Multithreaded for nonblocking.
 * @param vargp (void*) unused (NULL)
 */
void *inputThread(void *vargp) {
    while(!exit_flag) {
        char *ret = fgets(stdin_input, sizeof(stdin_input), stdin);
        if(ret == NULL) strcpy(stdin_input, "eof\n");
        stdin_input_read = true;
           
        struct timespec ts;
        ts.tv_sec  = 1E-2;
        ts.tv_nsec = 1E7;
        nanosleep(&ts, &ts);
    }
}

/**
 * @brief Manage input from socket-server. Multithreaded for nonblocking.
 * @param vargp (void*) unused (NULL)
 */
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
