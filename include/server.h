// ##########################################
// 
//   Author  -   Collin Thornton, Kazi Sherif
//   Email   -   collin.thornton@okstate.edu
//   Brief   -   Assignment 02 msg struct header
//   Date    -   10-27-20
//
// ########################################## 

#ifndef SERVER_H
#define SERVER_H

#include "msg.h"
#include "process.h"

/**
 * @brief run server process
 * @return (int) return code
 */
int server(void);



/**
 * @brief threaded function to interpret commands from client
 * @param vargp (void* -> Msg*) pointer to Msg structure
 * @return unused
 */
void* server_command_interpreter(void* vargp);

/**
 * @brief redirect filedescriptors
 * @param fdfrom (int) original file descriptor
 * @param fdto (int) new file descriptor
 */
void redirect(int fdfrom, int fdto);

/**
 * @brief execute a command with execvp()
 * @param proc (Process*) process to be executed
 */
void run(Process *proc);

/**
 * @brief execute a custom command
 * @param proc (Process*) process to be executed
 * @param outbuff (char*) output buffer (eventually send to client)
 */
void run_cmd_list(Process *proc, char* outbuff);

/**
 * @brief check if process is in custom command list
 * @param proc (Process*) process to be checked
 * @return (bool) true if process found. else false.
 */
bool in_cmd_list(Process *proc);

/**
 * @brief set flag to trigger shutdown of server
 */
void init_shutdown(void);

#endif // SERVER_H