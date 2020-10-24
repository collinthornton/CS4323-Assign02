// Build a Shell program
// to run the code
// sudo apt-get install libreadline-dev
// gcc a2.c -lreadline
// ./a.out



  
#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 

#define MAXCOM 1000 
// max number of letters to be supported 
#define MAXLIST 100 
// max number of commands to be supported 

// Clearing the shell using escape sequences 
#define clear() printf("\033[H\033[J") 

// Greeting shell during startup 
void initialize() 
{ 	 
	printf("\n\n\n\t**** SIMPLE SHELL ****"); 
	printf("\n\n"); 
	char* username = getenv("USER"); 
	printf("\n\n\nUSER is: @%s", username); 
	sleep(1); 
	clear(); 
} 

// Function to take input 
int Input(char* str) 
{ 
	char* buffer; 

	buffer = readline("\n>>> "); 
	if (strlen(buffer) != 0) { 
		add_history(buffer); 
		strcpy(str, buffer); 
		return 0; 
	} else { 
		return 1; 
	} 
} 

// Function to print Current Directory. 
void printDir(char*username) 
{ 
	char cwd[1024]; 
	getcwd(cwd, sizeof(cwd)); 

	printf("%s@CS4323shell:%s$", username, cwd); 
} 

// Function where the system command is executed 
void execArgs(char** parsed) 
{ 
	// Forking a child 
	pid_t p_id = fork(); 

	if (p_id == -1) { 
		printf("\n Unsuccessful forking "); 
		return; 
	} else if (p_id == 0) { 
		if (execvp(parsed[0], parsed) < 0) { 
			printf("\n Can't' execute command "); 
		} 
		exit(0); 
	} else { 
		// waiting for child to terminate 
		wait(NULL); 
		return; 
	} 
} 

// Function where the piped system commands is executed 
void execArgsPiped(char** parsed, char** parsedpipe) 
{ 
	// 0 is read end, 1 is write end 
	int pipefd[2]; 
	pid_t p1, p2; 

	if (pipe(pipefd) < 0) { 
		printf("\n Pipe could not be initialized"); 
		return; 
	} 
	p1 = fork(); 
	if (p1 < 0) { 
		printf("\n Could not fork "); 
		return; 
	} 

	if (p1 == 0) { 
		// Child 1 executing.. 
		// It only needs to write at the write end 
		close(pipefd[0]); 
		dup2(pipefd[1], STDOUT_FILENO); 
		close(pipefd[1]); 

		if (execvp(parsed[0], parsed) < 0) { 
			printf("\nCould not execute command 1"); 
			exit(0); 
		} 
	} else { 
		// Parent executing 
		p2 = fork(); 

		if (p2 < 0) { 
			printf("\nCould not fork"); 
			return; 
		} 

		// Child 2 executing.. 
		// It only needs to read at the read end 
		if (p2 == 0) { 
			close(pipefd[1]); 
			dup2(pipefd[0], STDIN_FILENO); 
			close(pipefd[0]); 
			if (execvp(parsedpipe[0], parsedpipe) < 0) { 
				printf("\nCould not execute command 2"); 
				exit(0); 
			} 
		} else { 
			// parent executing, waiting for two children 
			wait(NULL); 
			wait(NULL); 
		} 
	} 
} 

// Help command builtin 
void Help() 
{ 
	puts("\n***WELCOME TO MY SHELL HELP***"
		"\nList of Commands supported:"
		"\n>cd (change directory)"
		"\n>cd .. (go back one directory)"
		"\n>cd ~ (go back to home directory)"
		"\n>ls"
		"\n>exit"
		"\n>all other general commands available in UNIX shell"
		"\n>pipe handling"
		"\n>improper space handling"
		"\n>for checking background processes : ps aux | more");
	return; 
} 

// Function to execute builtin commands 
int ownCmdHandler(char** parsed) 
{ 
	int total_Cmds = 4, switchOwnArg = 0; 
	char* List_Cmds[total_Cmds]; 
	char* username; 

	List_Cmds[0] = "exit"; 
	List_Cmds[1] = "cd"; 
	List_Cmds[2] = "help"; 
	List_Cmds[3] = "hello"; 

	for (int i = 0; i < total_Cmds; i++) { 
		if (strcmp(parsed[0], List_Cmds[i]) == 0) { 
			switchOwnArg = i + 1; 
			break; 
		} 
	} 

	switch (switchOwnArg) { 
	case 1: 
		printf("\nSee Ya Later\n"); 
		exit(0); 
	case 2: 
		chdir(parsed[1]); 
		return 1; 
	case 3: 
		Help(); 
		return 1; 
	case 4: 
		username = getenv("USER"); 
		printf("\nHello %s.\n"
			"welcome to my simple shell."
			"\nUse the help when you want help :|", 
			username); 
		return 1; 
	default: 
		break; 
	} 

	return 0; 
} 

// function for finding pipe 
int parsePipe(char* str, char** str_pipe) 
{ 
	for (int i = 0; i < 2; i++) { 
		str_pipe[i] = strsep(&str, "|"); 
		if (str_pipe[i] == NULL) 
			break; 
	} 

	if (str_pipe[1] == NULL) 
		return 0; // returns zero if no pipe is found. 
	else { 
		return 1; 
	} 
} 

// function for parsing command words 
void parseSpace(char* str, char** parsed) 
{ 

	for (int i = 0; i < MAXLIST; i++) { 
		parsed[i] = strsep(&str, " "); 

		if (parsed[i] == NULL) 
			break; 
		if (strlen(parsed[i]) == 0) 
			i=i-1; 
	} 
} 

int processString(char* str, char** parsed, char** parsedpipe) 
{ 

	char* str_pipe[2]; 
	int pd = 0; 

	pd = parsePipe(str, str_pipe); 

	if (pd) { 
		parseSpace(str_pipe[0], parsed); 
		parseSpace(str_pipe[1], parsedpipe); 

	} else { 

		parseSpace(str, parsed); 
	} 

	if (ownCmdHandler(parsed)) 
		return 0; 
	else
		return 1 + pd; 
} 

int main() 
{ 
	char input[MAXCOM], *parsed[MAXLIST]; 
	char* parsed_Piped[MAXLIST]; 
	int Flag = 0; 
	char* username = init_shell();
	initialize(); 

	while (1) { 
		// print shell line 
		printDir(username); 
		// take input 
		if (Input(input)) 
		{
			continue; 
		}
		// process 
		Flag = processString(input, 
		parsed, parsed_Piped); 
		//execFlag = processString(inputString, parsedArgs, 
		//parsedArgsPiped);
		// execflag returns zero if there is no command 
		// or it is a builtin command, 
		// 1 if it is a simple command 
		// 2 if it is including a pipe. 

		// execute 
		if (Flag == 1) 
			execArgs(parsed); 

		if (Flag == 2) 
			execArgsPiped(parsed, parsed_Piped); 
	} 
	return 0; 
} 
