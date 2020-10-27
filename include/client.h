// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Brief   -   Assignment 02 Client include
//   Date    -   10-27-20
//
// ########################################## 

#ifndef CLIENT_H
#define CLIENT_H


/**
 * @brief Run the client process
 * @return (int) return code
 */
int client(void);

/**
 * @brief Manage input on stdin. Multithreaded for nonblocking.
 * @param vargp (void*) unused (NULL)
 */
void *inputThread(void *vargp);

/**
 * @brief Manage input from socket-server. Multithreaded for nonblocking.
 * @param vargp (void*) unused (NULL)
 */
void *socketReadThread(void *vargp);

#endif // CLIENT_H