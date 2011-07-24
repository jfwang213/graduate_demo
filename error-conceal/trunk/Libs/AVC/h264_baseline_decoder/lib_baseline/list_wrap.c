#include "list_wrap.h"
#ifndef NULL
#define NULL 0
#endif
void init_list_wrap(list_wrap* list_head)
{
	init_one_list(list_head->list);
}
void add_list_wrap(list_wrap* list_head,list_wrap* one_wrap)
{
	add_last(list_head->list,one_wrap->list);
}
void remove_list_wrap(list_wrap* one_wrap)
{
	remove_one(one_wrap->list);
}
list_wrap* get_first_wrap(list_wrap* list_head)
{
	if (list_head->list->aft_list_head == list_head->list)
	{
		return NULL;
	}
	else
	{
		return get_wrap_from_list(list_head->list->aft_list_head);
	}
}
list_wrap* get_next_wrap(list_wrap* list_head,list_wrap* list_now)
{
	if (NULL == list_now)
	{
		return get_first_wrap(list_head);
	}
	else if (list_now->list->aft_list_head == list_head->list)
	{
		return NULL;
	}
	else
	{
		return get_wrap_from_list(list_now->list->aft_list_head);
	}
}

char is_blank_wrap(list_wrap* list_head)
{
	return  is_blank(list_head->list);
}