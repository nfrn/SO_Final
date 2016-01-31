/*
 * list.c - implementation of the integer list functions 
 */


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "list.h"
#define STATUS_INFO_LINE_SIZE 50



list_t* lst_new()
{
   list_t *list;
   list = (list_t*) malloc(sizeof(list_t));
   list->first = NULL;
   return list;
}


void lst_destroy(list_t *list)
{
	struct lst_iitem *item, *nextitem;

	item = list->first;
	while (item != NULL){
		nextitem = item->next;
		free(item);
		item = nextitem;
	}
	free(list);
}


void insert_new_process(list_t *list, int pid, time_t starttime)
{
	lst_iitem_t *item;

	item = (lst_iitem_t *) malloc (sizeof(lst_iitem_t));
	item->pid = pid;
	item->starttime = starttime;
	item->endtime = 0;
	item->status = 0;
	item->next = list->first;
	list->first = item;
}


float update_terminated_process(list_t *list, int pid, time_t endtime,int status)
{
	float time_spent;
	lst_iitem_t *item;
	item = list->first;
	while(item!=NULL){
		if (item->pid ==pid){
			item->endtime=endtime;
			item->status=status;
			time_spent = (float)((item->endtime)-(item->starttime));
			break;
		}
   		item = item->next;
	} 
	return time_spent;
}


void lst_print(list_t *list)
{
	lst_iitem_t *item;
	float time_spent;
	printf("--Start:\n\n");
	item = list->first;
	char aux[STATUS_INFO_LINE_SIZE];
	char *childstatusmessage = NULL;
	

	while (item != NULL){
		childstatusmessage = (char*)malloc(STATUS_INFO_LINE_SIZE);
		childstatusmessage[0] = '\0';
		time_spent = (float)((item->endtime)-(item->starttime));
		if (WIFEXITED(item->status))
			snprintf(aux, sizeof(aux), "pid: %d exited normally; status=%d\n", item->pid, WEXITSTATUS(item->status)); /*snprintf= guarda o printf no buffer aux*/
		else
			snprintf(aux, sizeof(aux), "pid: %d terminated without calling exit\n", item->pid);
		strncat(childstatusmessage, aux, STATUS_INFO_LINE_SIZE); /*junta as duas strings*/
		if (childstatusmessage != NULL) 
			printf("%s", childstatusmessage);
		printf("\tStart: %s",ctime(&(item->starttime)));
		printf("\tEnd: %s",ctime(&(item->endtime)));
		printf("\tProcess: %d   ",item->pid);
		printf("Duration: %.2f s.\n\n", time_spent);
		free(childstatusmessage);
		item = item->next;
	}
	printf("-- end of list.\n"); 
}

