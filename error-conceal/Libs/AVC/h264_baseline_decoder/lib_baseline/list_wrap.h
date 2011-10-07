#ifndef _LIST_WRAP_H_
#define _LIST_WRAP_H_

#include "list_head.h"

typedef struct list_wrap
{
	list_item		list[1];
	void*				wrap_con;
}list_wrap;

void init_list_wrap(list_wrap* list_head);
void add_list_wrap(list_wrap* list_head,list_wrap* one_wrap);
void remove_list_wrap(list_wrap* one_wrap);
list_wrap* get_next_wrap(list_wrap* list_head,list_wrap* list_now);
char is_blank_wrap(list_wrap* list_head);
list_wrap* get_first_wrap(list_wrap* list_head);

#define get_wrap_from_list(plist_item) contain_of(plist_item,list_wrap,list)

#endif