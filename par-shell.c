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

#include<sys/stat.h>
#include<fcntl.h>

#include "commandlinereader.h"
#include "list.h"
#include "list_terminal.h"

#define EXIT_COMMAND "exit"
#define EXIT_GLOBAL "exit-global"
#define STATS_COMMAND "stats"
#define T_START "start"
#define T_EXIT "terminal-exit"
#define SERVER_PATH "/tmp/par-shell-in"

#define MAXARGS 7 /* Comando +  5 argumentos opcionais + espaco para NULL */
#define BUFFER_SIZE 100
#define MAXPAR 4

int numchildren=0;
int exit_thread=0; //sinal para a monitora fazer wait
pthread_mutex_t lock;
pthread_cond_t running1,monitoring1; 

FILE *log_file;
list_t2 *list_terminal;

int iteracao = 0;
float total_duration=0;

char newFileName[BUFFER_SIZE];
int newFileIndex = 0;

void apanhaCTRLC(int s){
	signal (SIGINT, apanhaCTRLC);
	lst_destroy_T(list_terminal);
	exit(EXIT_SUCCESS);
	}

void *monitora (list_t *list){ 
	float time_spent;
	while(1){
		pthread_mutex_lock(&lock);	//fechar mutex para leitura de variaveis partilhadas
		if(numchildren>0){
			int childpid, status;

			pthread_mutex_unlock(&lock); //nao se deve ter um mutex fechado numa funcao bloqueadora
			childpid = wait(&status);
			
			if (childpid < 0) {
				if (errno == EINTR)
					continue;
			
				else {
					perror("Error waiting for child.");
					exit (EXIT_FAILURE);
				}
			}
			time_t final_time=time(NULL);
			
			pthread_mutex_lock(&lock);
			pthread_cond_signal(&running1);
			
			time_spent=update_terminated_process(list, childpid, final_time, status);
			total_duration+=time_spent;
			fprintf(log_file,"%s %d\n","iteracao",iteracao);
			fflush(log_file);
			fprintf(log_file,"%s %d %s %.0f %s\n","pid:",childpid,"execution time:",time_spent,"s");
			fflush(log_file);
			fprintf(log_file,"%s %.0f\n","total execution time:", total_duration);
			fflush(log_file);
			iteracao++;
			numchildren --;
			pthread_mutex_unlock(&lock);

	    	}
		else{
			if (exit_thread==1){
				pthread_mutex_unlock(&lock);
				pthread_exit(NULL); //ao sair o ciclo deixa o mutex aberto
			}
			while (exit_thread == 0 && numchildren==0){
				pthread_cond_wait(&monitoring1,&lock);
			}
			pthread_mutex_unlock(&lock);
		}			
	}
}

int main (int argc, char** argv) {
	list_t *list= lst_new();
	list_terminal=lst_new_T();
	pthread_t th1;

	int lines = 0;
	int pipeIN;
	int pipe_stats;
  
	char *args[MAXARGS]; 
	char buffer[BUFFER_SIZE];
	char buffer_file[BUFFER_SIZE]; 

	//init global mutex and cond. variables
  	if (pthread_mutex_init(&lock, NULL) != 0)
    	{
       		perror("Mutex init failed\n");
        	exit(EXIT_FAILURE);
  	}
  	
  	if (pthread_cond_init(&running1, NULL)!=0){
  		perror("cond 1 failed\n");
  		exit(EXIT_FAILURE);
  	}
  	
  	if (pthread_cond_init(&monitoring1, NULL)!=0){
  		perror("cond 2 failed\n");
  		exit(EXIT_FAILURE);
  	} 
  	
	//create new thread
  	if( pthread_create(&th1,NULL, (void *)monitora,list) !=0 ) { 
  		fprintf(stderr,"Could not create Monitora thread.\n");
		exit(EXIT_FAILURE);
  	}
  	//_____________________________________________
  	//PIPE IN 
  	unlink(SERVER_PATH);
	
	if(mkfifo(SERVER_PATH, 0777) < 0){
		perror("mkfifo failed\n");
  		exit(EXIT_FAILURE);
  	}
  
  	
  	if((pipeIN= open(SERVER_PATH, O_RDONLY)) < 0){
		perror("opening pipe failed\n");  
  		exit(EXIT_FAILURE);
  	}
  	close(STDIN_FILENO);
  	dup(pipeIN);
	//________________________________________________
	//PIPE STATS
	unlink("/tmp/par-shell-stats");
	
	if(mkfifo("/tmp/par-shell-stats", 0777) < 0){
		perror("mkfifo(stats-out) failed\n");
  		exit(EXIT_FAILURE);
  	}
  	//_________________________________________________
  	//file
  	
	log_file = fopen("log.txt","ab+"); /*Abre o ficheiro modo leitura e escrita*/
	
	while(!feof(log_file)){
		fgets(buffer_file,BUFFER_SIZE,log_file);
		lines++;
	}
	iteracao=lines/3;
	sscanf(buffer_file,"total execution time: %f s", &total_duration);

	printf("ITERACAO: %d, TOTAL_TIME: %.0f, MAXPAR: %d \n",iteracao, total_duration,MAXPAR);
	printf("Shell on!\n");
	
	
	//___________________________________________________
	signal (SIGINT, apanhaCTRLC);
	//____________________________________________________
	while (1) { 
		int numargs;	
		numargs = readLineArguments(args, MAXARGS, buffer, BUFFER_SIZE);
		
		if (numargs == -1){
			close(pipeIN);
			if((pipeIN= open(SERVER_PATH, O_RDONLY)) < 0){
			      perror("opening pipe failed\n");  
			      exit(EXIT_FAILURE);
			}
			continue;
		}
		if (numargs == 0){
			continue;
		}
		if (strcmp(args[0], "start") == 0) {
			insert_new_process_T(list_terminal,atoi(args[1]));
			printf("Terminal[%d]: added to list.\n",atoi(args[1]));
			continue;    
		}

		if (strcmp(args[0], STATS_COMMAND) == 0) {
			if((pipe_stats= open("/tmp/par-shell-stats", O_WRONLY)) < 0){
				perror("opening pipe(stats) failed\n");  
				exit(EXIT_FAILURE);
			}
			sprintf(buffer,"Total childreen: %d Total time: %f\n",numchildren,total_duration);
			write(pipe_stats, buffer, strlen(buffer) );
			continue;    
		}

		if (strcmp(args[0], T_EXIT) == 0) {
			printf("Terminal exited\n");
			int pid = atoi(args[1]);
			printf("%d\n", pid);
			lst_destroy_one(list_terminal,pid);
			continue;
		}

		if (strcmp(args[0], EXIT_GLOBAL) == 0) {
			
			pthread_mutex_lock(&lock);
			exit_thread=1;// informs secondary thread that it can wait for child processes and exit.
			pthread_cond_signal(&monitoring1);
			pthread_mutex_unlock(&lock);
			pthread_join(th1,NULL); //espera pela thread acabar e só depois continua 

			lst_print(list);
			lst_destroy_T(list_terminal);

			fclose(log_file);
			lst_destroy(list);
			pthread_mutex_destroy(&lock);
			pthread_cond_destroy(&running1);
			pthread_cond_destroy(&monitoring1);
			
			close(pipe_stats);
			unlink("/tmp/par-shell-stats");
			close(pipeIN);
			unlink(SERVER_PATH);
			exit(EXIT_SUCCESS);
    		}
		
		//Else_______________________________________
		
		pthread_mutex_lock(&lock);
		while(numchildren >= MAXPAR){
			pthread_cond_wait(&running1,&lock);
		}
		pthread_mutex_unlock(&lock);
		int pid = fork();
		if (pid < 0) {
		 	perror("Failed to create new process.");
			exit(EXIT_FAILURE);
		}
		if (pid > 0) { 	  /* Codigo do processo pai */
			time_t start_time=time(NULL);
			pthread_mutex_lock(&lock);
			numchildren ++;
			insert_new_process(list, pid, start_time);
			pthread_cond_signal(&monitoring1);
			pthread_mutex_unlock(&lock);
		}
		else { /* Codigo do processo filho */
		  	sprintf(newFileName,"par-shell-out-%d.txt",getpid());
			printf("%s\n",newFileName);
			newFileIndex = open(newFileName,O_RDWR|O_CREAT|O_APPEND);
			dup2(newFileIndex,STDOUT_FILENO); 
			if (execv(args[0], args) < 0) {
				perror("Could not run child program. Child will exit.");
				exit(EXIT_FAILURE);
			}
		}
		
	} 
}
