#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../include/history.h"
// delecaring the maximun area of a command

#define MAX_LINE 80 
#define BUFFER_SIZE 500


char **history;
int num_allocated = 0, num_used = 0;


char** history_allocate() {
    history = (char**)malloc(10*sizeof(char*));
}




























char shellhis[10][BUFFER_SIZE]; 
// Showing the history size to for storing the command of the history
int total = 0;
// using the function to represent display 
void displayHistory()
{
   //printing the outline of shell 
    //printf("Showing the History details of  Shell command: \n");
    int first_index;
    int second_index = 0;
    int backgroundcount = total;
   
   // using  loop condition for iterating through commands
    for (first_index = 0; first_index<10;first_index++)
    {
        printf("%d.  ", backgroundcount);
        while (shellhis[first_index][second_index] != '\n' && shellhis[first_index][second_index] != '\0')
        {	
            printf("%c", shellhis[first_index][second_index]);
            second_index++;
        }
        printf("\n");
        second_index = 0;
        backgroundcount--;
        if (backgroundcount ==  0)
            break;
    }
    printf("\n");
    
   
} 
// using the function for restoring the commands from the shell interface 
// setting and arranging the paramter of the arrays 
int formatCommand(char buffInput[], char *arraysIndex[],int *flag)
{
   	int area; // Numbers of the character in command line 
    	int i;  
    	int beginning_part;  
    	int next_index = 0; 
    	int background;
 	area = read(STDIN_FILENO, buffInput, MAX_LINE);	
    beginning_part = -1;
    if (area == 0)
        exit(0);   
    if (area < 0)
    {
        printf("The user action is not read properly\n");
        exit(-1);  
    }
    
    for (i=0;i<area;i++)
    {
        switch (buffInput[i])
        {
            case ' ':
            case '\t' :               
                if(beginning_part != -1)
                {
                    arraysIndex[next_index] = &buffInput[beginning_part];    
                    next_index++;
                }
                buffInput[i] = '\0';
                beginning_part = -1;
                break;
                
            case '\n':                  
                if (beginning_part != -1)
                {
                    arraysIndex[next_index] = &buffInput[beginning_part];
                    next_index++;
                }
                buffInput[i] = '\0';
                arraysIndex[next_index] = NULL; 
                break;
                
            default :           
                if (beginning_part == -1)
                    beginning_part = i;
                if (buffInput[i] == '&')
                {
                    *flag  = 1; 
                    buffInput[i] = '\0';
                }
        }
    }
    
    arraysIndex[next_index] = NULL;

if(strcmp(arraysIndex[0],"History")==0)
        {		
               if(total>0)
		{
		
                displayHistory();
		}
		else
		{
		printf("\n Not applicable Commands found in the Background .Try again\n");
		}
		return -1;
        }

	else if (arraysIndex[0][0]-'!' ==0)
	{	int x = arraysIndex[0][1]- '0'; 
		int z = arraysIndex[0][2]- '0'; 
		
		if(x>total) 
		{
		printf("\nNot applicable Commands found in the Background .Try again\n");
		strcpy(buffInput,"Not a prefect command.please do it again ");
		} 
		else if (z!=-48)
		{
		printf("\nNot a prefect command.please do it. Enter <=!9 (buffer size is 10 along with current command)\n");
		strcpy(buffInput,"Not a prefect command.please do it again");
		}
		else
		{

			if(x==-15)
			{	 strcpy(buffInput,shellhis[0]);  
			}
			else if(x==0) 
			{	 printf("Please provide correct command");
				strcpy(buffInput,"Not a prefect command");
			}
			
			else if(x>=1) 
			{
				strcpy(buffInput,shellhis[total-x]);

			}
			
		}
	}
 for (i = 9;i>0; i--) 
       	strcpy(shellhis[i], shellhis[i-1]);
    
    strcpy(shellhis[0],buffInput); 
    total++;
	if(total>10)
	{ total=10;
	}
}
// main 
int main(void)
{
    char input_Buffer[MAX_LINE];
    char *arraysIndex[MAX_LINE/2 + 1];
    pid_t pid1;
    int a;
    int token;     
	while (1)
    {            
        token = 0;
        printf("kazi>");
        fflush(stdout);
        if(-1!=formatCommand(input_Buffer,arraysIndex,&token))
	{
		pid1 = fork();
        
        	if (pid1 < 0)
        	{
            
            		printf("Fork failed.\n");
            		exit (1);
        	}
        
       		 else if (pid1 == 0)
        	{

            		if (execvp(arraysIndex[0], arraysIndex) == -1)
           	 	{	
		
                		printf("Error executing command\n");
            		}
       		 }
        	else
        	{
            		a++;
           	 	if (token == 0)
           		 {
                		a++;
                		wait(NULL);
           		 }
        	}
   	 }
     }
}