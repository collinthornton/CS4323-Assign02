#ifndef CLIENT_SERVER_EV_H
#define CLIENT_SERVER_EV_H

#include <stdio.h> 
#include <netdb.h> 
#include <unistd.h>
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <pthread.h>

#include "msg.h"
#define sockA struct sockaddr
#define SOCKET_BUFF 5000

int socket_init();
int socket_connect();
Msg* socket_read();
int socket_write(Msg *msg);

void socket_close();

void *sockReadThread(void *vargp);

#endif // CLIENT_SERVER_EV_H