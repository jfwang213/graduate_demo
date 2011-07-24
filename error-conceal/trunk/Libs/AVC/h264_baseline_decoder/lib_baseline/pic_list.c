/*****************************************************************************
*
*  Open SVC Decoder developped in IETR image lab
*
*
*
*              Médéric BLESTEL <mblestel@insa-rennes.Fr>
*              Mickael RAULET <mraulet@insa-rennes.Fr>
*              http://www.ietr.org/
*
*
*
*
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*
*
* $Id$
*
**************************************************************************/

#include <string.h>
#include <stdio.h>

//AVC Files
#include "type.h"
#include "symbol.h"
#include "pic_list.h"
#include "picture_management.h"
#include "memory.h"
#include "layer_utils.h"
#include "assert.h"
#include "svc_type_util.h"
#include "stdlib.h"

extern void logStr(const char* format,...);



/**
This function allows to reorder the reference list when there is a gap in the decoding process.

@param mmo MMO structure which contains all information about memory management of the video.
@param MaxFrameNum Maximal number which can have a frame in the video.
@param num_ref_frames Number of reference frame in the current video.
@param RefPicListL0 Refernce list l0.
@param RefPicListL1 Refernce list l1.
*/
void fill_frame_num_gap(MMO *mmo, int baseLayerId, int MaxFrameNum, int num_ref_frames, 
						LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[],SLICE* currentSlice,int dq_id_display)
{
	//LAYER_MMO* LayerMmo = &(mmo->LayerMMO[baseLayerId]);
	char* layer_id_table = mmo->layer_id_table;
	int high_layer_id,temp_layer_id;
	DEP_MMO* depMmo = mmo->LayerMMO[baseLayerId].dep_mmo;
	LAYER_MMO* layerMmo = &(mmo->LayerMMO[baseLayerId]);
	LIST_MMO Current_pic[1];
	list_wrap* eclist_head;
	list_wrap* temp_list = NULL;
	int UnusedShortTermFrameNum = (depMmo -> prev_frame_num + 1) % MaxFrameNum;
	int i = (currentSlice -> frame_num - depMmo -> prev_frame_num - 1 + MaxFrameNum) % MaxFrameNum;	
	if (depMmo->last_nal_ref_idc == 0)
	{
		i++;
	}
	memset(Current_pic, 0, sizeof(LIST_MMO));

	//get eclist
	high_layer_id = baseLayerId+1;
	while(layer_id_table[high_layer_id] != -1 && layer_id_table[high_layer_id]>>4 == layer_id_table[baseLayerId]>>4)
	{
		if (is_blank_wrap(mmo->LayerMMO[high_layer_id].eclist))
		{
			break;
		}
		high_layer_id++;
	}
	high_layer_id--;
	eclist_head = mmo->LayerMMO[high_layer_id].eclist;
	for (; i > 0; i--){
		if (temp_list = get_next_wrap(eclist_head,temp_list))
		{
			Current_pic[0] = *(LIST_MMO*)temp_list->wrap_con;
		}
		else
		{
			Current_pic->memory_size = (mmo->MaxWidth+32)*(mmo->MaxHeight+32);
			Current_pic->pic_width = layerMmo->pcurr_sps->pic_width_in_mbs*16;
			Current_pic->pic_height = layerMmo->pcurr_sps->pic_height_in_map_units*16;
			if (dq_id_display>>4 == layer_id_table[baseLayerId]>>4)
			{
				allocate_pic_sample_memory(Current_pic);
				allocate_mv_memory(Current_pic);
				//because not add to display list
				Current_pic->displayed = 1;
			}
			else
			{
				allocate_mv_memory(Current_pic);
			}
		}
		Current_pic -> pic_id = UnusedShortTermFrameNum;
		Current_pic -> pic_num = UnusedShortTermFrameNum;
		Current_pic -> reference = 3;

		remove_short(mmo, depMmo, Current_pic -> pic_num,0);  
		remove_short(mmo, depMmo, Current_pic -> pic_num,1);
		if(depMmo -> ShortRefCount){
			memmove(&depMmo -> ShortRef[1], &depMmo -> ShortRef[0], depMmo -> ShortRefCount * sizeof(LIST_MMO));
		}
		

		depMmo -> ShortRef [0] = *Current_pic;
		depMmo -> ShortRefCount++;
		depMmo -> prev_frame_num = UnusedShortTermFrameNum;
		UnusedShortTermFrameNum = (depMmo -> prev_frame_num + 1) % MaxFrameNum;
		if( depMmo -> ShortRefCount +  depMmo -> LongRefCount >= num_ref_frames + 1){ 
			sliding_window(mmo, depMmo);               
		}
	}
	for (temp_layer_id = baseLayerId;temp_layer_id<=high_layer_id;temp_layer_id++)
	{
		temp_list = NULL;
		while (temp_list = get_next_wrap(mmo->LayerMMO[temp_layer_id].eclist,temp_list))
		{
			if (temp_layer_id != high_layer_id)
			{
				//high_layer_id is in short ref list
				release_pic_mv_memory((LIST_MMO*)temp_list->wrap_con);
			}
			remove_list_wrap(temp_list);
			free(temp_list->wrap_con);
			free(temp_list);
			temp_list = NULL;
		}
	}
}



/**
Compute POC of each frame.
*/
void PocComputation(SPS *sps, int IdrPicFlag, int nal_ref_idc, MMO *Mmo, 
					LAYER_MMO *LayerMmo, LIST_MMO *Current_pic,SLICE* current_slice)
{
	DEP_MMO* dep_mmo = LayerMmo->dep_mmo;
	int PicOrderCntMsb, TopFieldOrderCnt, BottomFieldOrderCnt;


	if(sps -> pic_order_cnt_type == 0){
		const int MaxPicOrderCntLsb = 1 << (sps -> log2_max_pic_order_cnt_lsb);

		if(IdrPicFlag == 5){
			dep_mmo -> prevPicOrderCntMsb = dep_mmo -> prevPicOrderCntLsb = 0;
		}

		if  ((current_slice -> pic_order_cnt_lsb < dep_mmo -> prevPicOrderCntLsb) &&
			((dep_mmo -> prevPicOrderCntLsb - current_slice -> pic_order_cnt_lsb) >= (MaxPicOrderCntLsb >> 1))){
				PicOrderCntMsb =  dep_mmo -> prevPicOrderCntMsb + MaxPicOrderCntLsb;
				dep_mmo -> prevPicOrderCntMsb = PicOrderCntMsb;
				dep_mmo -> prevPicOrderCntLsb = 0;

		}else if( (current_slice -> pic_order_cnt_lsb > dep_mmo -> prevPicOrderCntLsb) && 
			((current_slice -> pic_order_cnt_lsb - dep_mmo -> prevPicOrderCntLsb) > (MaxPicOrderCntLsb >> 1))){
				PicOrderCntMsb = dep_mmo -> prevPicOrderCntMsb - MaxPicOrderCntLsb;
				dep_mmo -> prevPicOrderCntMsb = PicOrderCntMsb;
		}else{
			PicOrderCntMsb = dep_mmo -> prevPicOrderCntMsb;
		}

		TopFieldOrderCnt = BottomFieldOrderCnt = PicOrderCntMsb + current_slice -> pic_order_cnt_lsb;

		if(current_slice -> field_pic_flag){
			BottomFieldOrderCnt += current_slice -> delta_pic_order_cnt_bottom;
		}
	}else if(sps -> pic_order_cnt_type == 1){
		int abs_frame_num, expected_delta_per_poc_cycle, expectedpoc;
		int i;

		if(sps -> num_ref_frames_in_pic_order_cnt_cycle != 0){
			abs_frame_num = current_slice -> frame_num_offset + current_slice -> frame_num;
		}else{
			abs_frame_num = 0;
		}
		if(nal_ref_idc == 0 && abs_frame_num > 0){
			abs_frame_num--;
		}

		expected_delta_per_poc_cycle = 0;
		for(i = 0; i < sps -> num_ref_frames_in_pic_order_cnt_cycle; i++){
			expected_delta_per_poc_cycle += sps -> offset_for_ref_frame[ i ]; //FIXME integrate during sps parse
		}

		if(abs_frame_num > 0){
			int poc_cycle_cnt = (abs_frame_num - 1) / sps -> num_ref_frames_in_pic_order_cnt_cycle;
			int frame_num_in_poc_cycle = (abs_frame_num - 1) % sps -> num_ref_frames_in_pic_order_cnt_cycle;

			expectedpoc = poc_cycle_cnt * expected_delta_per_poc_cycle;
			for(i = 0; i <= frame_num_in_poc_cycle; i++)
				expectedpoc = expectedpoc + sps -> offset_for_ref_frame[ i ];
		} else{
			expectedpoc = 0;
		}
		if(nal_ref_idc == 0){
			expectedpoc = expectedpoc + sps -> offset_for_non_ref_pic;
		}

		TopFieldOrderCnt = expectedpoc + current_slice -> delta_pic_order_cnt[0];
		BottomFieldOrderCnt = TopFieldOrderCnt + sps -> offset_for_top_to_bottom_field + current_slice -> delta_pic_order_cnt[1];

		if(current_slice -> field_pic_flag){
			BottomFieldOrderCnt += current_slice -> delta_pic_order_cnt[1];
		}
	}else{
		int tempPicOrderCnt;
		if(IdrPicFlag == 5){
			tempPicOrderCnt = 0;
		}else{
			if(nal_ref_idc) {
				tempPicOrderCnt = 2*(current_slice -> frame_num_offset + current_slice -> frame_num);
			}else {
				tempPicOrderCnt = 2*(current_slice -> frame_num_offset + current_slice -> frame_num) - 1;
			}
		}
		TopFieldOrderCnt = tempPicOrderCnt;
		BottomFieldOrderCnt = tempPicOrderCnt;
	}

	//Save the last decoded POC for comparison
	Mmo -> LastDecodedPoc = Current_pic -> poc;
	Current_pic -> poc = (TopFieldOrderCnt < BottomFieldOrderCnt)? TopFieldOrderCnt: BottomFieldOrderCnt;
}



/**
This function is used to determine initial picture orderings for reference picture in the decoding of B slices.


@param sps SPS structure of the current video.
@param nal_unit_type The type of the NAL unit.
@param nal_ref_idc Specifies if the current picture is a reference one.
@param mmo MMO structure which contains all information about memory management of the video.
@param Current_pic LIST_MMO structure with contains information on the current picture.
@param RefPicListL0 Refernce list l0.
@param RefPicListL1 Refernce list l1.
*/

void pic_order_process(SPS *sps, int IdrPicFlag, int nal_ref_idc, MMO *Mmo, LAYER_MMO *LayerMmo, 
					   LIST_MMO *Current_pic, LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[],
					   SLICE* current_slice,int dq_id_display)
{
	DEP_MMO* dep_mmo = LayerMmo->dep_mmo;
	sps->max_frame_num = 1 << (sps -> log2_max_frame_num);

	if(IdrPicFlag == 5){
		current_slice -> frame_num_offset = 0;
		dep_mmo -> prev_frame_num = 0;
	}else{
		if(current_slice -> frame_num < dep_mmo -> prev_frame_num)
			current_slice -> frame_num_offset = dep_mmo -> prev_frame_num_offset + sps->max_frame_num;
		else
			current_slice -> frame_num_offset = dep_mmo -> prev_frame_num_offset;
	}

	//Fill gap on every dep
	if ((!dep_mmo->frame_num_gap_check))
	{
		char base_layer_id;
		//get base layer id
		for (base_layer_id = 0;base_layer_id<8;base_layer_id++)
		{
			if (Mmo->layer_id_table[base_layer_id]>>4 == Mmo->layer_id_table[Mmo->LayerId]>>4)
			{
				break;
			}
		}
		dep_mmo->frame_num_gap_check = 1;
		if((current_slice -> frame_num != dep_mmo -> prev_frame_num) && 
			(current_slice -> frame_num != (dep_mmo -> prev_frame_num + 1) % sps->max_frame_num))
		{
			fill_frame_num_gap(Mmo, base_layer_id, sps->max_frame_num, sps -> num_ref_frames, 
				RefPicListL0, RefPicListL1,current_slice,dq_id_display);
		}
		else if ((current_slice -> frame_num == (dep_mmo -> prev_frame_num + 1) % sps->max_frame_num)&&
			dep_mmo->last_nal_ref_idc == 0)
		{
			//miss a frame
			fill_frame_num_gap(Mmo, base_layer_id, sps->max_frame_num, sps -> num_ref_frames, 
				RefPicListL0, RefPicListL1,current_slice,dq_id_display);
		}
		else
		{
			free_all_eclist(base_layer_id,dq_id_display,Mmo);
		}
		dep_mmo->last_nal_ref_idc = nal_ref_idc;
	}


	if (dep_mmo -> ShortRefCount + dep_mmo -> LongRefCount == sps -> num_ref_frames + 1){
		remove_unused(Mmo, dep_mmo);
	}

#ifdef ERROR_DETECTION
	//Avoid crashing when bitstream errors.
	//Error detection
	if(LayerMmo -> FreeMvMemory <= 0 || Mmo -> NumFreeAddress <= 0){
		//printf("Missing free mv place\n");
		remove_unused(Mmo, LayerMmo);
	}
#endif

	PocComputation(sps, IdrPicFlag, nal_ref_idc, Mmo, LayerMmo, Current_pic,current_slice);
}





/**
This function is used to initialize two tables for direct prediction for B slices.

@param aio_pstSlice The slice structure contains all information used to decode the next slice
@param RefPicListL0 Refernce list l0.
@param RefPicListL1 Refernce list l1.
@param Current_pic LIST_MMO structure with contains information on the current picture.
@param mmo MMO structure which contains all information about memory management of the video.
@param map_col_to_list0 Table used to determine the poc of the colocated macroblock.
@param map_col_to_list1 Table used to determine the poc of the colocated macroblock.
*/
void direct_ref_list_init( SLICE *aio_pstSlice, LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[], 
						  LIST_MMO *Current_pic, LAYER_MMO *LayerMmo)
{
	DEP_MMO* dep_mmo = LayerMmo->dep_mmo;

	int i, j;

	if(aio_pstSlice -> slice_type == SLICE_TYPE_I){
		Current_pic -> ref_count_l0 = 0;
	}

	if(aio_pstSlice -> slice_type != SLICE_TYPE_B){
		Current_pic -> ref_count_l1 = 0;
	}

	for(j = 0; j  <  dep_mmo -> RefCountL0; j++){
		Current_pic -> ref_poc_l0[j] = RefPicListL0[j] . poc;
	}

	Current_pic -> ref_count_l0 = dep_mmo -> RefCountL0;


	for(j = 0; j  < dep_mmo -> RefCountL1; j++){
		Current_pic -> ref_poc_l1[j] = RefPicListL1[j] . poc;
	}

	Current_pic -> ref_count_l1 = dep_mmo -> RefCountL1;


	if(aio_pstSlice -> slice_type != SLICE_TYPE_B || aio_pstSlice -> direct_spatial_mv_pred_flag)
		return;


	for(i = 0; i < RefPicListL1 -> ref_count_l0; i++){
		const int poc = RefPicListL1[0].ref_poc_l0[i];
		aio_pstSlice -> MapColList[i] = PART_NOT_AVAILABLE;
		for(j = 0; j < dep_mmo -> RefCountL0; j++){
			if (RefPicListL0[j] . poc == poc){
				aio_pstSlice -> MapColList [i] = j;
				break;
			}
		}
	}



	for(i = 0; i < RefPicListL1 -> ref_count_l1; i++){
		const int poc = RefPicListL1[0].ref_poc_l1[i];
		aio_pstSlice -> MapColList[16 + i] = PART_NOT_AVAILABLE;
		for(j = 0; j < dep_mmo -> RefCountL1; j++){
			if(RefPicListL1[j] . poc == poc){
				aio_pstSlice -> MapColList [16 + i] = j;
				break;
			}
		}
	}	
}



void execute_ref_base_pic_marking(MMO *Mmo,MMCO *Mmco,int index)
{
	int i,j;
	POC_PARAMS* poc = Mmo->poc_params;
	int pocLen = Mmo->nb_img_display-Mmo->num_free_poc_address;
	for (i = 0;i<index;i++)
	{
		switch (Mmco[i].opcode)
		{
		case 1:
			for (j = 0;j<pocLen;j++)
			{
				if (Mmco[i].short_frame_num == poc[j].Picture->pic_num &&
					poc[j].Picture->bBasePic)
				{
					poc[j].Picture->reference = 0;
					break;
				}
			}
			break;
		case 2:
			for (j = 0;j<pocLen;j++)
			{
				if (Mmco[i].long_term_frame_idx == poc[j].Picture->long_term &&
					poc[j].Picture->bBasePic)
				{
					poc[j].Picture->reference = 0;
				}
			}
			break;
		}
	}
}

void ref_pic_marking_idr(int baseLayerId,MMO *Mmo,LAYER_MMO* baseLayerMmo,LIST_MMO* baseLayerPic,
						 LIST_MMO *highestLayerPic,char bDealBaseLayer)
{
	DEP_MMO* dep_mmo = baseLayerMmo->dep_mmo;
	int depId = baseLayerId>>4;
	assert(baseLayerId>=0);
	remove_all(Mmo,dep_mmo);
	dep_mmo -> RefCountL0 = 0;
	dep_mmo -> prev_frame_num = 0;
	dep_mmo -> prev_frame_num_offset = 0;
	
	
	if (1 == dep_mmo->index)
	{
		dep_mmo->LongRef[0] = *highestLayerPic;
		dep_mmo->LongRef[0].pic_id = 0;
		dep_mmo->LongRefCount = 1;
		if (bDealBaseLayer)
		{
			dep_mmo->LongRefBase[0] = *baseLayerPic;
			dep_mmo->LongRefBase[0].pic_id = 0;
			dep_mmo->LongRefCountBase = 1;
			dep_mmo->LongRefBase[0].bBasePic = 1;
		}
	}
	else
	{
		if (bDealBaseLayer)
		{
			dep_mmo->ShortRef[0] = *baseLayerPic;
			dep_mmo->ShortRef[0].pic_id = baseLayerPic->pic_num;
			dep_mmo->ShortRef[0].bBasePic = 1;
			dep_mmo->ShortRef[1] = *highestLayerPic;
			dep_mmo->ShortRef[1].pic_id = highestLayerPic->pic_num;
			dep_mmo->ShortRefCount = 2;
		}
		else
		{
			dep_mmo->ShortRef[0] = *highestLayerPic;
			dep_mmo->ShortRef[0].pic_id = highestLayerPic->pic_num;
			dep_mmo->ShortRefCount = 1;
		}
		logStr("add short ref layerId:%d\torig layerId:%d\tshort ref count:%d\n",baseLayerId,
			highestLayerPic->ucLayerId,dep_mmo->ShortRefCount);
	}
}

/**
This fuction allows to execute the memory management for a right decoding of the current video.

@param mmco MMCO structure which contains the operatio to do on the short and long refernce list.
@param Curr LIST_MMO structure with contains information on the current picture.
@param mmo MMO structure which contains all information about memory management of the video.
*/
void execute_ref_pic_marking(SPS* sps,MMO *Mmo, int baseLayerId,int highestLayerId)
{

	char store_base_layer_flag;
	int bIdrFlag;
	int i, j;
	int current_is_long = 0;
	int depId = baseLayerId>>4;
	char bDealWithBase;
	LAYER_MMO *baseLayerMmo = &Mmo -> LayerMMO [baseLayerId];
	LAYER_MMO *highestLayerMmo = &(Mmo->LayerMMO[highestLayerId]);
	DEP_MMO* dep_mmo = baseLayerMmo->dep_mmo;
	MMCO *baseLayerMmco = dep_mmo -> BaseLayerMmco;
	MMCO *highestLayerMmco = dep_mmo->Mmco;

	LIST_MMO *baseLayerCurrPic = baseLayerMmo->currentPic;
	LIST_MMO *highestLayerCurrPic = highestLayerMmo->currentPic;
	baseLayerCurrPic->ucLayerId = baseLayerId;
	highestLayerCurrPic->ucLayerId = highestLayerId;
	store_base_layer_flag = dep_mmo->store_ref_base_pic_flag;
	bIdrFlag = dep_mmo->idr_flag;
	bDealWithBase = (baseLayerId != highestLayerId) && store_base_layer_flag;
	if (bDealWithBase)
	{
		logStr("deal with base layer ref pic marking\n");
	}
	if (bIdrFlag)
	{
		ref_pic_marking_idr(baseLayerId,Mmo,baseLayerMmo,baseLayerCurrPic,highestLayerCurrPic,
			bDealWithBase);
	}
	else
	{
		
		if (bDealWithBase)
		{
			for (i = 0;i<dep_mmo->baseLayerIndex;i++)
			{
				switch (baseLayerMmco[i].opcode)
				{
				case 1:
					baseLayerMmco[i].short_frame_num=(baseLayerCurrPic->pic_num-baseLayerMmco[i].short_frame_num-1) 
						& (sps->max_pic_num-1);
					remove_short(Mmo,dep_mmo,baseLayerMmco[i].short_frame_num,1);
					break;
				case 2:
					remove_long(Mmo, dep_mmo, baseLayerMmco[i] . long_term_frame_idx,1);
					break;
				}
			}
		}
		if (0 == dep_mmo->index)
		{
			if (dep_mmo->ShortRefCount + dep_mmo->LongRefCount + dep_mmo->LongRefCountBase >=
				(sps->num_ref_frames>1?sps->num_ref_frames:1))
			{
				remove_unused(Mmo,dep_mmo);
			}
			
		}
		for(i = 0; i < dep_mmo -> index; i++){
			switch(highestLayerMmco[i] . opcode){
		case 1:
			highestLayerMmco[i].short_frame_num = (baseLayerCurrPic->pic_num-highestLayerMmco[i].short_frame_num-1)&
				(sps->max_pic_num-1);
			remove_short(Mmo, dep_mmo, highestLayerMmco[i] . short_frame_num,0);
			break;

		case 2:
			remove_long(Mmo, dep_mmo, highestLayerMmco[i] . long_term_frame_idx,0);
			break;

		case 3:
			highestLayerMmco[i].short_frame_num = (baseLayerCurrPic->pic_num-highestLayerMmco[i].short_frame_num-1)&
				(sps->max_pic_num-1);
			remove_long(Mmo, dep_mmo, highestLayerMmco[i] . long_term_frame_idx,0);
			remove_long(Mmo, dep_mmo, highestLayerMmco[i] . long_term_frame_idx,1);
			short_to_long(Mmo, dep_mmo, highestLayerMmco[i] . short_frame_num, highestLayerMmco[i] . long_term_frame_idx,0);
			short_to_long(Mmo, dep_mmo, highestLayerMmco[i] . short_frame_num, highestLayerMmco[i] . long_term_frame_idx,1);
			break;

		case 4:
			// just remove the long term which index is greater than new max
			for(j = highestLayerMmco[i] . long_term_frame_idx; j < 16; j++){
				remove_long(Mmo, dep_mmo, j,0);
				remove_long(Mmo, dep_mmo, j,1);
			}
			break;

		case 5:
			remove_all(Mmo, dep_mmo);
			highestLayerCurrPic -> pic_num = 0;
			highestLayerCurrPic -> poc = 0;
			if (bDealWithBase)
			{
				baseLayerCurrPic->pic_num = 0;
				baseLayerCurrPic->poc = 0;
			}
			dep_mmo->prev_frame_num = 0;
			dep_mmo->prev_frame_num_offset = 0;
			break;

		case 6:
			remove_long(Mmo, dep_mmo, highestLayerMmco[i] . long_term_frame_idx,0);
			remove_long(Mmo, dep_mmo, highestLayerMmco[i] . long_term_frame_idx,1);
			highestLayerCurrPic -> long_term = 1;
			dep_mmo -> LongRef[highestLayerMmco[i] . long_term_frame_idx]= *highestLayerCurrPic;
			dep_mmo -> LongRef[highestLayerMmco[i] . long_term_frame_idx] . pic_id = highestLayerMmco[i] . long_term_frame_idx;
			dep_mmo -> LongRefCount++;
			if (bDealWithBase)
			{
				baseLayerCurrPic->long_term = 1;
				dep_mmo->LongRefBase[highestLayerMmco[i].long_term_frame_idx] = *baseLayerCurrPic;
				dep_mmo->LongRefBase[highestLayerMmco[i].long_term_frame_idx].pic_id = highestLayerMmco[i] . long_term_frame_idx;
				dep_mmo->LongRefBase[highestLayerMmco[i].long_term_frame_idx].bBasePic = 1;
				dep_mmo->LongRefCountBase++;
			}
			current_is_long = 1;
			break;
			}
		}
		dep_mmo -> index = 0;
		if(!current_is_long){
			if(dep_mmo -> ShortRefCount){
				memmove(&dep_mmo -> ShortRef[1], &dep_mmo -> ShortRef[0], dep_mmo -> ShortRefCount * sizeof(LIST_MMO));
			}

			dep_mmo -> ShortRef[0] = *highestLayerCurrPic;
			dep_mmo -> ShortRefCount++;
			logStr("add short ref layerId:%d\torig layerId:%d\tshort ref count:%d\n",baseLayerId,
				highestLayerCurrPic->ucLayerId,dep_mmo->ShortRefCount);
			if (bDealWithBase)
			{
				if (dep_mmo->ShortRefCount)
				{
					memmove(&dep_mmo -> ShortRef[1], &dep_mmo -> ShortRef[0], dep_mmo -> ShortRefCount * sizeof(LIST_MMO));
				}
				dep_mmo->ShortRef[0] = *baseLayerCurrPic;
				dep_mmo->ShortRefCount++;
				dep_mmo->ShortRef[0].bBasePic = 1;
			}
		}
	}
}


LIST_MMO* findPocFrame(LIST_MMO* list,int poc,int listNum)
{
	int i;
	for (i = 0;i<listNum;i++)
	{
		if (list[i].poc == poc)
		{
			return list+i;
		}
	}
	return NULL;
}


/**
This function is used to generate the to reference lists.


@param refPicLXl0 Refernce list l0.
@param refPicLXl1 Refernce list l1.
@param Current_pic LIST_MMO structure with contains information on the current picture.
@param slice_type Slice type of the current slice.
*/
void fill_default_ref_list(DEP_MMO* dep_mmo, LIST_MMO *refPicLXl0,LIST_MMO *refPicLXl1,LIST_MMO *current,
						   int slice_type,char bUserBaseLayer)
{
	int shortRefCount = 0;
	int i;
	int smallest_poc_greater_than_current = -1;
	LIST_MMO sorted_short_ref[32];

	


	if(slice_type == SLICE_TYPE_B){
		int out_i;
		int limit = MIN_POC;///INT_MIN;

		// sort frame according to poc in B Slice 
		for(out_i = 0; out_i < dep_mmo -> ShortRefCount; out_i++)
		{
			int best_i = 0;//INT_MIN;
			int best_poc = MAX_POC;//= ;//INT_MAX;
			int findOne = 0;
			for(i = 0; i < dep_mmo -> ShortRefCount; i++){
				const int poc =  dep_mmo -> ShortRef[i].poc;
				if (dep_mmo->ShortRef[i].bBasePic != bUserBaseLayer)
				{
					continue;
				}
				if(poc > limit && poc < best_poc){
					best_poc = poc;
					best_i = i;
					findOne = 1;
				}
			}

			if (!findOne)
			{
				break;
			}

			limit = best_poc;
			sorted_short_ref[out_i] = dep_mmo -> ShortRef[best_i];
			if (-1 == smallest_poc_greater_than_current) {
				if (dep_mmo -> ShortRef[best_i] . poc >= current -> poc) {
					smallest_poc_greater_than_current = out_i;
				}
			}
		}
		shortRefCount = out_i;
	}

	if(slice_type == SLICE_TYPE_B){
		int i,j,index;
		// list0
		index = 0;
		for (j = smallest_poc_greater_than_current-1;j>=0&&index<dep_mmo->RefCountL0;j--)
		{
			if (sorted_short_ref[j].reference == 3)
			{
				refPicLXl0[index] = sorted_short_ref[j];
				refPicLXl0[index++].pic_id = sorted_short_ref[j].pic_num;
			}
		}
		for (j = smallest_poc_greater_than_current;j<shortRefCount&&index<dep_mmo->RefCountL0;j++)
		{
			if (sorted_short_ref[j].reference == 3)
			{
				refPicLXl0[index] = sorted_short_ref[j];
				refPicLXl0[index++].pic_id = sorted_short_ref[j].pic_num;
			}
		}
		//long ref
		for(i = 0; i < 16 && index<dep_mmo->RefCountL0; i++){
			if(dep_mmo -> LongRef[i] . reference == 3) 
			{
				if (bUserBaseLayer && dep_mmo->LongRefBase[i].reference == 3)
				{
					refPicLXl0[index  ] = dep_mmo -> LongRef[i];
					refPicLXl0[index++] . pic_id = i;
				}
				else
				{
					refPicLXl0[index  ] = dep_mmo -> LongRefBase[i];
					refPicLXl0[index++] . pic_id = i;
				}
				
			}
		}
		dep_mmo->RefCountL0 = index;
		//list1
		index = 0;
		for (j = smallest_poc_greater_than_current;j<shortRefCount&&index<dep_mmo->RefCountL1;j++)
		{
			if (sorted_short_ref[j].reference == 3)
			{
				refPicLXl1[index] = sorted_short_ref[j];
				refPicLXl1[index++].pic_id = sorted_short_ref[j].pic_num;
			}
		}
		for (j = smallest_poc_greater_than_current-1;j>=0&&index<dep_mmo->RefCountL1;j--)
		{
			if (sorted_short_ref[j].reference == 3)
			{
				refPicLXl1[index] = sorted_short_ref[j];
				refPicLXl1[index++].pic_id = sorted_short_ref[j].pic_num;
			}
		}
		//long ref
		for(i = 0; i < 16 && index<dep_mmo->RefCountL1; i++){
			if(dep_mmo -> LongRef[i] . reference == 3) 
			{
				if (bUserBaseLayer && dep_mmo->LongRefBase[i].reference == 3)
				{
					refPicLXl1[index  ] = dep_mmo -> LongRefBase[i];
					refPicLXl1[index++] . pic_id = i;
				}
				else
				{
					refPicLXl1[index  ] = dep_mmo -> LongRef[i];
					refPicLXl1[index++] . pic_id = i;
				}
				
			}
		}
		dep_mmo->RefCountL1 = index;
		
		if((smallest_poc_greater_than_current <= 0 || smallest_poc_greater_than_current >= dep_mmo -> ShortRefCount) && (1 < index)){
			// swap the two first elements of L1 when
			// L0 and L1 are identical
			LIST_MMO temp = refPicLXl1[0];
			refPicLXl1[0] = refPicLXl1[1];
			refPicLXl1[1] = temp;
		}
	}else{
		int index = 0;

		for(i = 0; i < dep_mmo -> ShortRefCount && index < dep_mmo -> RefCountL0; i++){
			if(dep_mmo -> ShortRef[i] . reference != 3 || 
				dep_mmo->ShortRef[i].bBasePic != bUserBaseLayer){
				continue; //FIXME refernce field shit
			}
			refPicLXl0[index] = dep_mmo -> ShortRef[i];
			refPicLXl0[index++] . pic_id = dep_mmo -> ShortRef[i] . pic_num;
		}

		for(i = 0; i < 16 && index < dep_mmo -> RefCountL0; i++){
			if(dep_mmo -> LongRef[i] . reference == 3)
			{
				if (bUserBaseLayer && dep_mmo->LongRefBase[i].reference == 3)
				{
					refPicLXl0 [index] = dep_mmo -> LongRefBase[i];
					refPicLXl0 [index++] . pic_id = i;
				}
				else
				{
					refPicLXl0 [index] = dep_mmo -> LongRef[i];
					refPicLXl0 [index++] . pic_id = i;
				}
			}
		}
		if(index < dep_mmo -> RefCountL0){
			memset(&refPicLXl0[index], 0, sizeof(LIST_MMO) * (dep_mmo -> RefCountL0 - index));
		}
		dep_mmo->RefCountL0 = index;
	}
}
