#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include "commandlinereader.h"
#include<sys/stat.h>

#define BUFFER_SIZE 100
#define MAXARGS 7
#define STATS_COMMAND_ "stats\n"
#define EXIT_COMMAND_ "exit\n"
#define EXIT_GLOBAL_ "exit-global\n"
#define T_EXIT_ "terminal-exit "
#define T_START_ "start "



int main (int argc, char** argv) {
	int pipe_out;
	int pipe_stats;
	
	int myPid;
	char SmyPid[sizeof(int)];
	
	char FULLBUFFER[BUFFER_SIZE];
	char *token;
	
	
	//pipe out
	if ((pipe_out = open(argv[1],O_WRONLY)) < 0){
		perror("opening pipe failed\n");  
		exit(EXIT_FAILURE);
	}
	
	//PID_String
	myPid= getpid();
	sprintf(SmyPid, "%d", myPid);
	
	//Send Terminal_PID
/**/	char t_pid[BUFFER_SIZE];
	strcat(t_pid, T_START_);
	strcat(t_pid, SmyPid);
	strcat(t_pid, "\n");
	write(pipe_out,t_pid, strlen(t_pid));
	
	while (1) { 
		char input[BUFFER_SIZE];
/**/		while(fgets(input,BUFFER_SIZE,stdin)==NULL){
			continue;
		}
	  

		if (strcmp(input, STATS_COMMAND_) == 0) {
			write(pipe_out,input, strlen(input));
			if ((pipe_stats= open("/tmp/par-shell-stats", O_RDONLY)) < 0){
			      perror("opening pipe failed\n");  
			      exit(EXIT_FAILURE);
			}
		    
			read(pipe_stats, FULLBUFFER ,BUFFER_SIZE );
			token = strtok(FULLBUFFER,"\n");
		    
			printf("Total time: %s\n",token);
			continue;
		}
		
		else if (strcmp(input, EXIT_COMMAND_) == 0) {
			char t_exit[strlen(T_EXIT_)+ strlen(SmyPid)];
			strcat(t_exit, T_EXIT_);
			strcat(t_exit, SmyPid);
			printf(t_exit);
			write(pipe_out,t_exit, strlen(t_exit));
			close(pipe_out);
			close(pipe_stats);
			exit(EXIT_SUCCESS);
		}
		
		else if (strcmp(input, EXIT_GLOBAL_) == 0) {
			write(pipe_out,EXIT_GLOBAL_, strlen(EXIT_GLOBAL_)); 
			close(pipe_out);
			close(pipe_stats);
			exit(EXIT_SUCCESS);
		}
		else{
			write(pipe_out,input, strlen(input));
			continue;
			
		}
		

		
		
		
	}
}

		
		
			
