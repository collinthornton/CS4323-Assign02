#ifndef CLIENT_H
#define CLIENT_H



int client(void);

void *inputThread(void *vargp);
void *socketReadThread(void *vargp);

#endif // CLIENT_H