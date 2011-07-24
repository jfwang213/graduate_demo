#ifndef _LIST_HEAD_H_
#define _LIST_HEAD_H_


typedef struct list_item
{
	struct list_item* pre_list_head;
	struct list_item* aft_list_head;
}list_item;

//there is a blank head in the list

void init_one_list(list_item* plist_head);
void add_last(list_item* plist_head,list_item* to_add);
void add_first(list_item* plist_head,list_item* to_add);
char is_blank(list_item* plist_item);
void remove_one(list_item* plist_item);

#define contain_of(plist_item,class_name,list_name) (class_name *)((int)(plist_item)-(int)(((class_name*)(NULL))->list_name))

#endif