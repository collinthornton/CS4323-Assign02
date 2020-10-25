#ifndef SERVER_H
#define SERVER_H

#include "msg.h"

int server(void);
Msg* server_command_interpreter(Msg *msg);

#endif // SERVER_H