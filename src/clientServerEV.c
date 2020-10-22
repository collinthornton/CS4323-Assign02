#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#define sockA struct sockaddr

int main(int argc, char const *argv[]){

    int PORT = 8080;
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
    bzero(&toClient, sizeof(toClient));

    // Function for chatting between client and server 
    chat(connfd,toClient);  
} 

void chat(int sockfd, char *out){ 
    char buff[1024]; 
    char out[1024] = out;
    char message[1024];
    int n; 

    // Clear buffer
    bzero(buff, 1024); 

    // Message from client, needs to be sent elsewhere
    message = read(sockfd, buff, sizeof(buff)); 

    // Clear buffer
    bzero(buff, 1024); 
  
    // Send message to client
    write(sockfd, out, sizeof(buff)); 
  
    //Close server if "exit" 
    if (strncmp("exit", buff, 4) == 0){ 
        printf("Close Server\n"); 
        close(sockfd);
    }
} 