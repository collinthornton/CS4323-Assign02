#include <time.h>

#include "../include/server.h"
#include "../include/background.h"
#include "../include/clientServerEV.h"

//#define EXEC_SERVER

bool loop = true;
bool in_foreground = false;

Msg *resp;

ProcessList background_list, history;

int foreground_stdin[2];

int stdin_bak;
int stderr_bak;
int stdout_bak;

int server(void) {
    pthread_t cmd_int;
    Msg *msg = NULL;

    stdin_bak  = dup(STDIN_FILENO);
    stderr_bak = dup(STDERR_FILENO);
    stdout_bak = dup(STDOUT_FILENO);

    #ifdef VERBOSE
    printf("|----- SERVER:\t%d, %d\r\n", getppid(), getpid());
    #endif // VERBOSE

    msg = NULL;

    #ifndef EXEC_SERVER
    socket_init();
    #endif // EXEC_SERVER

    process_list_init(&background_list, NULL, NULL);
    process_list_init(&history, NULL, NULL);
    

    while(loop) {
        #ifdef EXEC_SERVER
        char dir_str[1024];
        getcwd(dir_str, sizeof(dir_str));
        printf("%s$> ", dir_str);
        fflush(stdout);

        char input_buff[1024];
        bzero(input_buff, sizeof(input_buff));
        
        fgets(input_buff, sizeof(input_buff), stdin);
        input_buff[strlen(input_buff)-1] = '\0';

        msg = msg_allocate(input_buff, NULL, NULL);
        #else
        msg = socket_read();
        #endif // EXEC_SERVER

        if(msg != NULL ) {

            Process history_node;
            process_init(&history_node, msg);
            strcpy(history_node.exec, msg->cmd);
            process_list_add_node(&history, &history_node);
            process_rem(&history_node);


            bool only_whitespace = true;

            for(int i=strlen(msg->cmd)-1; i>=0; --i) {
                if(msg->cmd[i] == ' ' || msg->cmd[i] == '\t') msg->cmd[i] = '\0';
                else {
                    only_whitespace = false;
                    break;
                }
            }

            char dir[500];
            bzero(dir, sizeof(dir));
            getcwd(dir, sizeof(dir));

            if(only_whitespace) {
                resp = msg_allocate(msg->cmd, "\0", dir);
                resp->show_prompt = true;
                
                #ifndef EXEC_SERVER
                socket_write(resp);
                #else
                msg_deallocate(resp);
                #endif // EXEC_SERVER
                
                resp = NULL;
                msg_deallocate(msg);
            }
            else if(in_foreground) {
                // pipe to stdin of executing process
                strcat(msg->cmd, "\n");
                close(foreground_stdin[0]);
                write(foreground_stdin[1], msg->cmd, sizeof(msg->cmd));
                if(strcmp(msg->cmd, "eof\n") == 0) {
                    close(foreground_stdin[1]);
                   // printf("closed pipe\r\n");
                }
                msg_deallocate(msg);
            }
            else {
                bool background = (msg->cmd[strlen(msg->cmd)-1] == '&') ? true : false;

                resp = msg_allocate(msg->cmd, "\0", dir);
                resp->show_prompt = background;
                
                #ifndef EXEC_SERVER
                socket_write(resp);
                #else
                msg_deallocate(resp);
                #endif // EXEC_SERVER
                
                resp = NULL;

                pthread_create(&cmd_int, NULL, server_command_interpreter, (void*)msg);
            }
        }


        if(resp != NULL) {
            char dir[500];
            bzero(dir, sizeof(dir));
            getcwd(dir, sizeof(dir));

            getcwd(dir, sizeof(dir));
            strcpy(resp->dir, dir);

            resp->show_prompt = true;

            #ifdef EXEC_SERVER
            printf("%s\r\n", resp->ret);
            msg_deallocate(resp);
            #else
            socket_write(resp);
            #endif // EXEC_SERVER

            resp = NULL;
        }


        // Sleep for 0.01 second
        struct timespec ts;
        ts.tv_sec  = 1E-2;
        ts.tv_nsec = 1E7;
        nanosleep(&ts, &ts);
    }

    redirect(STDOUT_FILENO, stdout_bak);
    redirect(STDERR_FILENO, stderr_bak);
    redirect(STDIN_FILENO, stdin_bak);

    process_list_del_list(&background_list);
    process_list_del_list(&history);


    #ifdef VERBOSE
    printf("Exiting server\r\n");
    #endif // VERBOSE

    return 0;
}


void* server_command_interpreter(void* vargp) {
    Msg *msg = (Msg*)vargp;

    int num_pipes = 2;
    for(int i=0; i<strlen(msg->cmd); ++i) {
        if(msg->cmd[i] == '|') ++num_pipes;
    }

    bool background = (msg->cmd[strlen(msg->cmd)-1] == '&') ? true : false;

    if(background) {
        msg->cmd[strlen(msg->cmd)-1] = '\0';
    }

    Process procs[num_pipes-1];
    int pipes[num_pipes][2];
    for(int i=0; i<num_pipes; ++i) pipe(pipes[i]);


    // MAKE A COPY OF THE COMMAND
    char cmd[1024];
    strcpy(cmd, msg->cmd);

    char *pos = cmd;
    char *tmp_cmd = (char*)calloc(1024, sizeof(char));
    


    for(int i=0; i<num_pipes-1; ++i) {
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
    for(int i=1; i<num_pipes; ++i) {
        if(!background) {
            in_foreground = true;
            foreground_stdin[0] = pipes[i-1][0];
            foreground_stdin[1] = pipes[i-1][1];
        } 

        #ifdef VERBOSE
        printf("num pipes: %d\r\n", num_pipes-1);
        printf("cmd: %s\r\n", cmd);
        printf("procs_exec: %s\r\n", procs[i].exec);
        for(int j=0; j<procs[i].num_args; ++j) {
            printf("procs_args: %s\r\n", procs[i].args[j]);
        }
        printf("\r\n\r\n");
        #endif // VERBOSE

        if(in_cmd_list(&procs[i-1])) {
            // HANDLE OWN COMMANDS
            if(background) {
                procs[i-1].pid = getpid();
                background_place_proc(&background_list, &procs[i-1]);
            }

            char outbuff[5000];
            bzero(outbuff, sizeof(outbuff));

            run_cmd_list(&procs[i-1], outbuff);
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
                    redirect(STDERR_FILENO, pipes[i][1]);
                    redirect(STDOUT_FILENO, pipes[i][1]);
                    close(pipes[i][1]);

                    redirect(STDIN_FILENO, pipes[i-1][0]);
                    close(pipes[i-1][0]);

                    run(&procs[i-1]);
                    process_rem(&procs[i-1]);
                    exit(0);
                }

                default:
                    // PARENT
                    close(pipes[i][1]);
                    close(pipes[i-1][0]);
                    if(background) {
                        procs[i-1].pid = child;
                        background_place_proc(&background_list, &procs[i-1]);
                    } else {
                        wait(NULL);
                    }
                    break;
            }
        }
    }
    in_foreground = false;

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

    strcpy(msg->ret, buff);

    for(int i=0; i<num_pipes-1; ++i) {
        process_rem(&procs[i]);
    }

    resp = msg_allocate(msg->cmd, msg->ret, NULL);
    resp->show_prompt = true;
}




void redirect(int fdfrom, int fdto) {
    dup2(fdto, fdfrom);
}

void run(Process *proc) {
    execvp(proc->exec, proc->args);
    exit(-1);
    // HANDLE FAILURE
}


void run_cmd_list(Process *proc, char *outbuff) {
    if(strcmp(proc->exec, "exit") == 0) {
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
            return;
        }      
        strcpy(outbuff, "cd"); 
        return; 
    }
    else if(strcmp(proc->exec, "history") == 0) {
        // HANDLE HISTORY
        process_list_to_string(&history, outbuff);
        return;
    }
    else if(strcmp(proc->exec, "jobs") == 0) {
        background_update_procs(&background_list);
        process_list_to_string(&background_list, outbuff);
       // printf("%s\r\n", outbuff);
        return;
    }
    else if(strcmp(proc->exec, "help") == 0) {
        sprintf(outbuff, "CMD\t\tDESCRIPTION\r\n\r\n");
        sprintf(outbuff + strlen(outbuff), "exit\t\texit shell\r\n");
        sprintf(outbuff + strlen(outbuff), "cd\t\tchange directory\r\n");
        sprintf(outbuff + strlen(outbuff), "history\t\tdisplay command history\r\n");
        sprintf(outbuff + strlen(outbuff), "jobs\t\tlist jobs in background\r\n");
        sprintf(outbuff + strlen(outbuff), "help\t\tdisplay this help message\r\n\r\n");
        return;
    }
}


bool in_cmd_list(Process *proc) {
    int num_cmds = 5;
    const char* cmd_list[num_cmds];
    cmd_list[0] = "exit";
    cmd_list[1] = "cd";
    cmd_list[2] = "history";
    cmd_list[3] = "jobs";
    cmd_list[4] = "help";

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

