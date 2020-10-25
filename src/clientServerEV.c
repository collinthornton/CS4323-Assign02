#include "../include/clientServerEV.h"

int PORT = 8081;
int sockfd, connfd;
pthread_t tid = 0;

bool sock_exit_flag = false;
char sock_input[SOCKET_BUFF];
int sock_queue_len = 0;


int socket_init(void) {
    int len; 
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

    pthread_create(&tid, NULL, sockReadThread, NULL);
}


Msg* socket_read() {
    if(sock_queue_len <= 0) return NULL;
    --sock_queue_len;

    char buff[SOCKET_BUFF];
    strcpy(buff, sock_input);

    Msg *msg = msg_deserialize(buff);
    return msg;    
}


int socket_write(Msg *msg) {
    char buff[SOCKET_BUFF];
    msg_serialize(msg, buff);

    write(connfd, buff, sizeof(buff)); 
    msg_deallocate(msg);
}

void socket_close() {
    if(tid != 0) pthread_cancel(tid);
}

// THREAD TO READ SOCKET
void *sockReadThread(void *vargp) {
    while(!sock_exit_flag) {
        char buff[SOCKET_BUFF];
        bzero(buff, SOCKET_BUFF);

        if(read(connfd, buff, sizeof(buff)) == -1) {
            perror("client:  socket closed");
            exit(1);
        }
        strcpy(sock_input, buff);
        ++sock_queue_len;
    }
}