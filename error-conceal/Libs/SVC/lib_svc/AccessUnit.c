#include <stdlib.h>
#include "list_head.h"
#include "svc_type.h"
#include "svc_type_util.h"
#include "AccessUnit.h"
#include "free_data.h"
void put_nal(access_unit* paccess_unit,NAL* pNal)
{
	add_last(paccess_unit->plist_head,pNal->nal_list);
	if (is_vcl_nal(pNal))
	{
		paccess_unit->plast_vcl_nal = pNal;
	}
	paccess_unit->nal_num++;
}

void put_first_nal(access_unit* paccess_unit,NAL* pNal)
{
	add_last(paccess_unit->plist_head->aft_list_head,pNal->nal_list);
	if (is_vcl_nal(pNal) && !paccess_unit->plast_vcl_nal)
	{
		paccess_unit->plast_vcl_nal = pNal;
	}
	paccess_unit->nal_num++;
}

NAL* get_last_nal(access_unit* current_access_unit)
{
	if (is_blank(current_access_unit->plist_head))
	{
		return NULL;
	}
	return contain_of(current_access_unit->plist_head->pre_list_head,NAL,nal_list);
}
NAL *remove_last_nal(access_unit* current_access_unit)
{
	NAL* res = get_last_nal(current_access_unit);
	if (res)
	{
		remove_one(res->nal_list);
	}
	return res;
}

char is_end_access_unit(access_unit* paccess_unit,NAL* next_nal)
{
	NAL* plast_nal = paccess_unit->plast_vcl_nal;
	SLICE* plast_slice;
	SLICE* pnext_slice;
	LIST_MMO* plast_picture;
	LIST_MMO* pnext_picture;

	if (is_blank(paccess_unit->plist_head) || NULL == paccess_unit->plast_vcl_nal)
	{
		return 0;
	}
	plast_slice = plast_nal->pSlice;
	pnext_slice = next_nal->pSlice;
	plast_picture = plast_nal->current_picture;
	pnext_picture = next_nal->current_picture;
	if (!is_vcl_nal(next_nal))
	{
		return 0;
	}
	if (plast_nal->DependencyId > next_nal->DependencyId)
	{
		return 1;
	}
	if (plast_nal->DependencyId == next_nal->DependencyId && 
		plast_nal->QualityId>=next_nal->QualityId)
	{
        // this equal is not correct for all h.264 svc code but for the situation in which one nal has one frame
		return 1;
	}
	//if (plast_slice->frame_num!= pnext_slice->frame_num)
	//{
	//	return 1;
	//}
	//if (plast_nal->NalRefIdc != next_nal->NalRefIdc && 
	//	!(plast_nal->NalRefIdc && next_nal->NalRefIdc))
	//{
	//	return 1;
	//}
	if (plast_nal->IdrFlag!= next_nal->IdrFlag)
	{
		return 1;
	}
	if (plast_nal->IdrFlag && next_nal->IdrFlag&&
		plast_slice->idr_pic_id!= pnext_slice->idr_pic_id)
	{
		return 1;
	}
	return 0;
	
}

void remove_useless_layer(int dq_id,access_unit* au)
{
	NAL* temp = NULL,*next;
	while (temp = get_next_nal(au,temp))
	{
		if (temp->DqId>dq_id)
		{
			next = get_pre_nal(au,temp);
			remove_one(temp->nal_list);
			free(temp->pSlice);
			free_nal(temp);
			temp = next;
			au->nal_num--;
		}
	}
}



NAL* get_first_nal(access_unit* current_access_unit)
{
	if (is_blank(current_access_unit->plist_head))
	{
		return NULL;
	}
	return get_nal_from_list(current_access_unit->plist_head->aft_list_head);
}


NAL* get_pre_nal(access_unit* current_access_unit,NAL* last_nal)
{
	if (NULL == last_nal)
	{
		if (is_blank(current_access_unit->plist_head))
		{
			return NULL;
		}
		return get_nal_from_list(current_access_unit->plist_head->pre_list_head);
	}
	if (last_nal->nal_list->pre_list_head == current_access_unit->plist_head)
	{
		return NULL;
	}
	return get_nal_from_list(last_nal->nal_list->pre_list_head);
}
NAL* get_next_nal(access_unit* current_access_unit,NAL* last_nal)
{
	if (NULL == last_nal)
	{
		return get_first_nal(current_access_unit);
	}
	if (last_nal->nal_list->aft_list_head ==  current_access_unit->plist_head)
	{
		return NULL;
	}
	return get_nal_from_list(last_nal->nal_list->aft_list_head);
}

void init_access_unit(access_unit* paccess_unit)
{
	init_one_list(paccess_unit->plist_head);
	paccess_unit->plast_vcl_nal = NULL;
	paccess_unit->nal_num = 0;
}
