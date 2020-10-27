#ifndef SERVER_H
#define SERVER_H

#include "msg.h"
#include "process.h"

int server(void);
void init_shutdown(void);

void* server_command_interpreter(void* vargp);

void redirect(int fdfrom, int fdto);
void run(Process *proc);
void run_cmd_list(Process *proc, char* outbuff);

bool in_cmd_list(Process *proc);

#endif // SERVER_H