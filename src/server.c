#include <time.h>

#include "../include/server.h"
#include "../include/background.h"
#include "../include/clientServerEV.h"

// #define EXEC_SERVER

bool loop = true;

int stdin_bak;
int stderr_bak;
int stdout_bak;


int server(void) {
    stdin_bak  = dup(STDIN_FILENO);
    stderr_bak = dup(STDERR_FILENO);
    stdout_bak = dup(STDOUT_FILENO);

    #ifdef VERBOSE
    printf("|----- SERVER:\t%d, %d\r\n", getppid(), getpid());
    #endif // VERBOSE

    socket_init();

    while(loop) {
        //char dir_str[1024];
        //getcwd(dir_str, sizeof(dir_str));
        //printf("%s$> ", dir_str);
        //fflush(stdout);

        char input_buff[1024];
        bzero(input_buff, sizeof(input_buff));
        
        //fgets(input_buff, sizeof(input_buff), stdin);
        //input_buff[strlen(input_buff)-1] = '\0';
        Msg *msg;

        //msg = msg_allocate(input_buff, NULL, NULL);
        do {
            msg = socket_read();

            // Sleep for 0.01 second
            struct timespec ts;
            ts.tv_sec  = 1E-2;
            ts.tv_nsec = 1E7;
            nanosleep(&ts, &ts);
        } while(msg == NULL);


        server_command_interpreter(msg);
        //printf("%s\r\n", msg->ret);
        
        // HISTORY
        // INTERPRETER
        // FOREGROUND
        // BACKGROUND

        
        char dir[500];
        getcwd(dir, sizeof(dir));
        strcpy(msg->dir, dir);

        msg->show_prompt = true;

        socket_write(msg);
        //msg_deallocate(msg);

    }

    redirect(STDOUT_FILENO, stdout_bak);
    redirect(STDERR_FILENO, stderr_bak);
    redirect(STDIN_FILENO, stdin_bak);

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


    // MAKE A COPY OF THE COMMAND
    char cmd[1024];
    strcpy(cmd, msg->cmd);

    char *pos = cmd;
    char *tmp_cmd = (char*)calloc(1024, sizeof(char));

    for(int i=0; i<num_pipes; ++i) {
        char *oldpos;
        if(i==0) oldpos = pos;
        else oldpos = pos+2;

        pos = strchr(pos+1, '|');

        if(pos != NULL) strncpy(tmp_cmd, oldpos, strlen(oldpos)-strlen(pos)-1);
        else strncpy(tmp_cmd, oldpos, strlen(cmd)-(strlen(oldpos)-strlen(cmd)-1));

        Msg *tmp_msg = msg_allocate(tmp_cmd, NULL, NULL);
        process_init(&procs[i], tmp_msg);
        msg_deallocate(tmp_msg);
    }

    free(tmp_cmd);


    // ITERATE THROUGH THE JOBS
    for(int i=0; i<num_pipes; ++i) {
        #ifdef VERBOSE
        printf("num pipes: %d\r\n", num_pipes-1);
        printf("cmd: %s\r\n", cmd);
        printf("procs_exec: %s\r\n", procs[i].exec);
        for(int j=0; j<procs[i].num_args; ++j) {
            printf("procs_args: %s\r\n", procs[i].args[j]);
        }
        printf("\r\n\r\n");
        #endif // VERBOSE

        if(in_cmd_list(&procs[i])) {
            // HANDLE OWN COMMANDS
            char outbuff[5000];
            bzero(outbuff, sizeof(outbuff));

            run_cmd_list(&procs[i], outbuff);
            write(pipes[i][1], outbuff, sizeof(outbuff));
            close(pipes[i][1]);
        }
        else {
            // HANDLE SYSTEM COMMANDS
            pid_t child = fork();
            switch(child) {
                case -1:
                    // ERROR:

                    break;

                case 0: {
                    // CHILD
                    close(pipes[i][0]);
                    redirect(STDOUT_FILENO, pipes[i][1]);
                    redirect(STDERR_FILENO, pipes[i][1]);

                    if(i > 0) {
                        redirect(STDIN_FILENO, pipes[i-1][0]);
                    }

                    run(&procs[i]);
                    exit(0);
                }

                default:
                    // PARENT
                    close(pipes[i][1]);
                    if(i > 0) close(pipes[i-1][0]);

                    break;
            }
        }
    }

    char buff[5000];

    bzero(buff, sizeof(buff));
    close(pipes[num_pipes-1][1]);
    while(read(pipes[num_pipes-1][0], buff, sizeof(buff)) != 0) { 
        // Sleep for 0.001 second
    struct timespec ts;
        ts.tv_sec  = 1E-3;
        ts.tv_nsec = 1E6;
        nanosleep(&ts, &ts);
    }
    close(pipes[num_pipes-1][0]);

    //printf("%s\r\n", buff);

    strcpy(msg->ret, buff);

    for(int i=0; i<num_pipes; ++i) {
        process_rem(&procs[i]);
    }
    return msg;
}




void redirect(int fdfrom, int fdto) {
    dup2(fdto, fdfrom);
    close(fdto);
}

void run(Process *proc) {
    execvp(proc->exec, proc->args);
    exit(-1);
    // HANDLE FAILURE
}


void run_cmd_list(Process *proc, char *outbuff) {
    if(strcmp(proc->exec, "q") == 0) {
        // HANDLE EXIT
        strcpy(outbuff, "shutdown");
        init_shutdown();
        return;
    }
    else if(strcmp(proc->exec, "cd") == 0) {
        // HANDLE CD
        if(proc->num_args != 2) {
            strcpy(outbuff, "cd: incorrect number of arguments");
            return;
        }
        if(chdir(proc->args[1]) != 0) {
            strcpy(outbuff ,"cd: not a valid folder");
            fflush(stdout);
            return;
        }      
        strcpy(outbuff, "cd"); 
        return; 
    }
    else if(strcmp(proc->exec, "history") == 0) {
        // HANDLE HISTORY
        //! WRITE TO PIPE
    }
}


bool in_cmd_list(Process *proc) {
    int num_cmds = 3;
    const char* cmd_list[3];
    cmd_list[0] = "q";
    cmd_list[1] = "cd";
    cmd_list[2] = "history";

    for(int i=0; i<num_cmds; ++i) {
        if(strcmp(cmd_list[i], proc->exec) == 0) return true;
    }
    return false;
}


void init_shutdown(void) {
    // SEND SHUTDOWN MSG TO CLIENT
    loop = false;
}



#ifdef EXEC_SERVER
int main() {
    server();
    exit(0);
}
#endif // EXEC_SERVER

