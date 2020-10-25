#include <time.h>

#include "../include/server.h"
#include "../include/background.h"
#include "../include/clientServerEV.h"

//#define EXEC_SERVER

int server(void) {
    #ifdef VERBOSE
    printf("|----- SERVER:\t%d, %d\r\n", getppid(), getpid());
    #endif // VERBOSE

    socket_init();
    
    while(1) {
        Msg *msg;
        do {
            msg = socket_read();

            // Sleep for 0.01 second
            struct timespec ts;
            ts.tv_sec  = 1E-2;
            ts.tv_nsec = 1E7;
            nanosleep(&ts, &ts);
        } while(msg == NULL);


        server_command_interpreter(msg);

        
        // HISTORY
        // INTERPRETER
        // FOREGROUND
        // BACKGROUND

        
        char dir[500];
        getcwd(dir, sizeof(dir));
        strcpy(msg->dir, dir);

        msg->show_prompt = true;

        socket_write(msg);
    }


    //process_list_test();

    //background_test();

    //sleep(2);
    printf("Exiting server\r\n");

    return 0;
}


Msg* server_command_interpreter(Msg *msg) {
    int num_pipes = 1;
    for(int i=0; i<strlen(msg->cmd); ++i) {
        if(msg->cmd[i] == '|') ++num_pipes;
    }

    //bool background = false;
    //if(strchr(msg->cmd, '&') != NULL) background = true;

    Process procs[num_pipes];
    int pipes[num_pipes][2];
    for(int i=0; i<num_pipes; ++i) pipe(pipes[i]);

    for(int i=0; i<num_pipes; ++i) {
        char cmd[1024];
        strcpy(cmd, msg->cmd);

        char *tmp;
        if(i == 0) tmp = strtok(cmd, "|");
        else tmp = strtok(NULL, "|");

        if(tmp == NULL) tmp = msg->cmd;

        Msg *tmp_msg = msg_allocate(tmp, NULL, NULL);
        Process tmp_proc;
        process_init(&tmp_proc, tmp_msg);

        pid_t child;
        if((child = fork()) == 0) {
            close(pipes[i][0]);
            dup2(pipes[i][1], 1);
            dup2(pipes[i][1], 2);
            close(pipes[i][1]);

            if(i > 0) {
                close(pipes[i-1][1]);
                dup2(0, pipes[i][0]);
            }


            if(strcmp(tmp_proc.exec, "q") == 0) {
                // HANDLE EXIT
            }
            else if(strcmp(tmp_proc.exec, "cd") == 0) {
                // HANDLE CD
                if(tmp_proc.num_args != 2) {}
            }
            else if(strcmp(tmp_proc.exec, "history") == 0) {
                // HANLDE HISTORY
            }
            else {
                execvp(tmp_proc.exec, tmp_proc.args);
                // HANDLE FAILURE
            }

            if(i>0) close(pipes[i-1][0]);
        }
        else {
            // CONNECT PIPES
        }


        msg_deallocate(tmp_msg);
    }

    Process proc;
    process_init(&proc, msg);
    strcpy(msg->ret, proc.ret);

    return msg;
    /*
    
    if(strcmp(proc.exec, "q") == 0) {
        socket_close();
        exit(0);
    }
    else if(strcmp(proc.exec, "cd") == 0) {
        if(proc.num_args != 2) {
            strcpy(proc.ret, "cd: incorrect number of arguments");
            strcpy(msg->ret, proc.ret);

            return msg;
        }
        if(chdir(proc.args[1]) != 0) {
            strcpy(proc.ret, proc.args[1]);
            strcat(proc.ret, " not a valid folder");
        }
    } else {
        pid_t child;
        int pipefd[2];
        pipe(pipefd);

        if((child = fork()) == 0) {
            close(pipefd[0]);
            dup2(pipefd[1], 1);
            dup2(pipefd[1], 2);
            close(pipefd[1]);

            execvp(proc.exec, proc.args);
            char buff[100];
            strcpy(proc.ret, proc.exec);
            strcat(proc.ret, " command not found");
        } else {

            char buff[5000];
            bzero(buff, sizeof(buff));
            close(pipefd[1]);
            while(read(pipefd[0], buff, sizeof(buff)) != 0) { 
                // Sleep for 0.001 second
                struct timespec ts;
                ts.tv_sec  = 1E-3;
                ts.tv_nsec = 1E6;
                nanosleep(&ts, &ts);
            }
            close(pipefd[0]);

            strcpy(proc.ret, buff);
        }
    }
    */
}







#ifdef EXEC_SERVER
int main() {
    server();
    exit(0);
}
#endif // EXEC_SERVER

