#ifndef _ACCESSUNIT_H_
#define _ACCESSUNIT_H_

#include "list_head.h"
#include "type.h"
#include "svc_type.h"

typedef struct
{
	list_item plist_head[1];
	
	NAL*		plast_vcl_nal;
	int		nal_num;
}access_unit;



void free_access_unit(access_unit* paccess_unit);

void put_nal(access_unit* paccess_unit,NAL* pNal);
void put_first_nal(access_unit* paccess_unit,NAL* pNal);
char is_end_access_unit(access_unit* paccess_unit,NAL* next_nal);
NAL* get_last_nal(access_unit* current_access_unit);
NAL *remove_last_nal(access_unit* current_access_unit);
NAL* get_first_nal(access_unit* current_access_unit);
NAL* get_next_nal(access_unit* current_access_unit,NAL* last_nal);
void init_access_unit(access_unit* paccess_unit);
NAL* get_pre_nal(access_unit* current_access_unit,NAL* last_nal);


#endif