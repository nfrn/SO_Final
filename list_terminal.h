#ifndef LIST_TERMINAL_H
#define LIST_TERMINAL_H

#include <stdlib.h>
#include <stdio.h>




/* lst_iitem - each element of the list points to the next element */
typedef struct lst_iitem2 {
   int pid;
   struct lst_iitem2 *next;
} lst_iitem_t2;

/* list_t */
typedef struct {
   lst_iitem_t2 * first;
} list_t2;



/* lst_new - allocates memory for list_t and initializes it */
list_t2* lst_new_T();

/* lst_destroy - free memory of list_t and all its items */
void lst_destroy_T(list_t2 *);

void lst_destroy_one(list_t2 *, int pid);

/* insert_new_process - insert a new item with process id and its start time in list 'list' */
void insert_new_process_T(list_t2 *list, int pid);

int hasAlreadyTerminal(list_t2 *list, int pid);

#endif