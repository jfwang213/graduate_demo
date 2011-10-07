#include "list_head.h"



void remove_one(list_item* plist_item)
{
	list_item	*pre,*aft;
	pre = plist_item->pre_list_head;
	aft = plist_item->aft_list_head;
	pre->aft_list_head = aft;
	aft->pre_list_head = pre;
}

void init_one_list(list_item* plist_head)
{
	plist_head->aft_list_head = plist_head;
	plist_head->pre_list_head = plist_head;
}


void add_last(list_item* plist_head,list_item* to_add)
{
	to_add->pre_list_head = plist_head->pre_list_head;
	plist_head->pre_list_head = to_add;
	to_add->pre_list_head->aft_list_head = to_add;
	to_add->aft_list_head = plist_head;
}


void add_first(list_item* plist_head,list_item* to_add)
{
	to_add->aft_list_head = plist_head->aft_list_head;
	to_add->aft_list_head->pre_list_head = to_add;
	plist_head->aft_list_head = to_add;
	to_add->pre_list_head = plist_head;
}

char is_blank(list_item* plist_item)
{
	return plist_item->aft_list_head == plist_item;
}