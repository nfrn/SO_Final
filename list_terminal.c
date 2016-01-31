#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list_terminal.h"
#define STATUS_INFO_LINE_SIZE 50

#include <signal.h>


list_t2* lst_new_T()
{
   list_t2 *list;
   list = (list_t2*) malloc(sizeof(list_t2));
   list->first = NULL;
   return list;
}


void lst_destroy_T(list_t2 *list)
{
	struct lst_iitem2 *item, *nextitem;

	item = list->first;
	while (item != NULL){
		nextitem = item->next;
		kill(item->pid, SIGKILL);
		free(item);
		item = nextitem;
	}
	free(list);
}
void lst_destroy_one(list_t2 *list, int pid)
{
	struct lst_iitem2 *item, *nextitem, *previtem;

	item = list->first;
	while (item != NULL){
		if (item->pid==pid){
		      if(item ==list->first){
			     printf("1");
			     free(item);
			     continue;
		      }
		      previtem->next = item->next;
		      free(item);
		}
		previtem=item;
		nextitem = item->next;
		item = nextitem;
	}
}


void insert_new_process_T(list_t2 *list, int pid)
{
	lst_iitem_t2 *item;
	item = (lst_iitem_t2 *) malloc (sizeof(lst_iitem_t2));
	item->pid = pid;
	item->next = list->first;
	list->first = item;
}
int hasAlreadyTerminal(list_t2 *list, int pid)
{	
	struct lst_iitem2 *item, *nextitem;
	item = list->first;
	while (item != NULL){
	      lst_iitem_t2 *item;
	      if(item->pid == pid){return 0;}
	      nextitem = item->next;
	      item = nextitem;
	}
	return 1;
}

