#include <stdio.h> 
#include <netdb.h> 
#include <unistd.h>
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 

#include "../include/msg.h"

#define sockA struct sockaddr

void chat(int sockfd, char *out);

int main(int argc, char const *argv[]){

    int PORT = 8081;
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
    
    // Create and verify socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("Socket Failed\n"); 
        exit(0); 
    } else {
        printf("Socket Success\n"); 
    }
    
    // Clear servaddr
    bzero(&servaddr, sizeof(servaddr)); 
  
    // IP and PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // Binding socket
    if ((bind(sockfd, (sockA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("Bind Failed\n"); 
        exit(0); 
    } else {
        printf("Bind Success\n"); 
    }
  
    // LIstening
    if ((listen(sockfd, 10)) != 0) { 
        printf("Listen Failed\n"); 
        exit(0); 
    } else {
        printf("Listen Success\n"); 
    }

    len = sizeof(cli); 
  
    // Accept client and begin chat
    connfd = accept(sockfd, (sockA*)&cli, &len); 
    if (connfd < 0) { 
        printf("Accept Failed\n"); 
        exit(0); 
    } else {
        printf("Accept Success\n"); 
    }
  
    // Message to send
    char toClient[1024];
    strcpy(toClient, "ACK");

    // Function for chatting between client and server 
    chat(connfd,toClient);  
} 

void chat(int sockfd, char *out){ 
    char buff[1024]; 
    char outbuff[1024];
    char message[1024];
    int n; 

    printf("Entering loop\r\n");
    while(1) {
        //strcpy(outbuff, out);

        // Clear buffer
        bzero(buff, 1024); 

        // Message from client, needs to be sent elsewhere
        if(read(sockfd, buff, sizeof(buff)) == -1) {
            printf("Socket closed\r\n");
            exit(0);
        }
        strcpy(message, buff);

        Msg *msg = msg_deserialize(message);
        printf("cmd:\t%s\r\n", msg->cmd);
        printf("ret:\t%s\r\n", msg->ret);
        printf("dir:\t%s\r\n", msg->dir);
        printf("show prompt:\t%d\r\n\r\n", msg->show_prompt);


        // Clear buffer
        bzero(buff, 1024); 
    
        // Send message to client
        printf("Sending ACK\r\n");

        strcpy(msg->ret, "ACK");

        char dir[100];
        getcwd(dir, sizeof(dir));

        strcpy(msg->dir, dir);
        msg->show_prompt = false;

        msg_serialize(msg, buff);

        write(sockfd, buff, sizeof(buff)); 
        msg_deallocate(msg);

    
        //Close server if "exit" 
        if (strncmp("q", message, 1) == 0){ 
            printf("Close Server\n"); 
            close(sockfd);
            exit(0);
        }
    }
} 