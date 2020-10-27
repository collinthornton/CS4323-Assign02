// ##########################################
// 
//   Author  -   Collin Thornton, Ethan Vascellaro
//   Email   -   collin.thornton@okstate.edu
//   Brief   -   Assignment 02 socket include
//   Date    -   10-27-20
//
// ########################################## 


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


/**
 * @brief Initialize the socket
 * @return (int) return code
 */
int socket_init();

/**
 * @brief Read from the socket (nonblocking)
 * @return (Msg*) NULL if nothing to read
 */
Msg* socket_read();

/**
 * @brief Write to the socket (blocking)
 * @return (int) return code
 */
int socket_write(Msg *msg);

/**
 * @brief Shutdown the socket
 */
void socket_close();


/**
 * @brief Thread to managing blocking read calls
 * @param vargp (void*) unused (NULL)
 */
void *sockReadThread(void *vargp);

#endif // CLIENT_SERVER_EV_H