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

//#ifdef ERROR_CONCEALMENT
//AVC File
#include "type.h"
#include "data.h"
#include "init_data.h"

//SVC Files
#include "svc_type.h"
#include "WriteBackSVC.h"
#include "ErrorConcealment.h"
#include "layer_utils.h"
#include "string.h"
#include "slice_data_B_svc.h"
#include "slice_data_I_svc.h"
#include "slice_data_P_svc.h"
#include "stdlib.h"
#include "Coordonates.h"
#include "init_data.h"
#include "slice_data_svc.h"
#include "svc_type_util.h"
#include "init_data.h"
#include "SliceHeaderExtension.h"
#include "stdio.h"
#include "pic_list.h"
#include "memory.h"
#include "FinishFrameSVC.h"
#include "SliceGroupMap.h"
#include "slice_header.h"
#include "pic_error_conceal.h"
#include "Slice_data_avc.h"
#include "fill_caches_svc.h"
#include "Direct_prediction.h"


/**
This function sets all macroblock data to those given.
*/
void ErrorSetMbType(const SPS *Sps, SLICE *Slice, RESIDU *Residu, unsigned char *SliceTable, int MbType)
{
	short iCurrMbAddr;
	short PicSizeInMbs = Sps -> PicSizeInMbs;

	for (iCurrMbAddr = 0; iCurrMbAddr < PicSizeInMbs ; iCurrMbAddr ++) {
		if (SliceTable [iCurrMbAddr] == 255){
			init_residu(&Residu[iCurrMbAddr]);
			SliceTable [iCurrMbAddr] = Slice -> slice_num;
			Residu[iCurrMbAddr] . MbType = MbType;
			//Residu[iCurrMbAddr] . Intra16x16PredMode = 3;
		}
	}
}


/**
This function fills macrblock data for a P/B frame in an AVC layer.
*/
void AVCPBErrorConcealment(const SPS *Sps, SLICE *Slice, unsigned char *SliceTable, RESIDU *Residu,
						 int MbType, short *MvL0, short *MvL1, short *RefL0, short *RefL1)
{

	short iCurrMbAddr;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;
	const short PicSizeInMbs = Sps -> PicSizeInMbs;


	__declspec(align(64)) short RefCacheL0 [48];
	__declspec(align(64)) short MvCacheL0 [48][2];


	memset(RefCacheL0, 0, 48 * sizeof(short));
	memset(MvCacheL0, 0, 96 * sizeof(short));


	for (iCurrMbAddr = 0; iCurrMbAddr < PicSizeInMbs ; iCurrMbAddr ++) {
		if (SliceTable [iCurrMbAddr] == 255){
			short iMb_x, iMb_y;
			GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);

			init_residu(&Residu[iCurrMbAddr]);
			SliceTable [iCurrMbAddr] = Slice -> slice_num;
			Residu[iCurrMbAddr] . MbType = MbType;
			write_back_motion_full_ref( Slice -> b_stride, Slice -> b8_stride,
				&MvL0[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], MvCacheL0,
				&RefL0[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], RefCacheL0 );
			write_back_motion_full_ref( Slice -> b_stride, Slice -> b8_stride,
				&MvL1[(iMb_x << 3) + iMb_y * (Slice -> b_stride << 2)], MvCacheL0,
				&RefL1[(iMb_x << 2) + iMb_y * (Slice -> b_stride << 1)], RefCacheL0 );
		}
	}
}



/*
This function sets macroblock type when error detection is activated.
*/
void AVCErrorConcealment(NAL *Nal, const SPS *Sps, SLICE *Slice, unsigned char *SliceTable, RESIDU *Residu,
					  short *MvL0, short *MvL1, short *RefL0, short *RefL1)
{
	if (Nal -> ErrorDetection && Slice -> mb_xy < Sps -> PicSizeInMbs){
		//AVC Error concealment
		//Contrary to the SVC case, as motion vector are compute in the same time that the VLD,
		// we have to set its to a value.
		if( Slice -> slice_type != SLICE_TYPE_I){
			//Mb type is determined according to the slice type.
			//For AVC layer, motion vector and reference index are set here.
			int MbType = P_Skip - Slice -> slice_type; 
			AVCPBErrorConcealment(Sps, Slice, SliceTable, Residu, MbType, MvL0, MvL1, RefL0, RefL1);
		}else {
			//For intra frame, Mb type is set to a 16x16 macroblock with DC prediction
			ErrorSetMbType(Sps, Slice, Residu, SliceTable, INTRA_16x16); 
		}
	}
}


/*
This function sets macroblock type when error detection is activated.
*/
void SVCErrorConcealment(NAL *Nal, const SPS *Sps, SLICE *Slice, unsigned char *SliceTable, RESIDU *Residu)
{
	if (Nal -> ErrorDetection && Slice -> mb_xy < Sps -> PicSizeInMbs){
		//SVC Error concealment
		if (Slice -> slice_type != SLICE_TYPE_I){
			int MbType = P_Skip - Slice -> slice_type; 
			ErrorSetMbType(Sps, Slice, Residu, SliceTable, MbType); //Skip macroblock in all case
		}else if(Nal -> DqId){
			int MbType = I_BL; 
			ErrorSetMbType(Sps, Slice, Residu, SliceTable, MbType); //I_BL for spatial and SNR enhancement
		}
	}
}
//#endif

void error_conceal_init_slice(NAL* curr_nal,SLICE* pslice,DATA* block,RESIDU* residu,RESIDU* 
							  base_residu,int picWidthInMbs,int mb_num,SPS* curr_sps)
{
	short iMb_x,iMb_y;
	short curr_mb_num;
	for (curr_mb_num = 0;curr_mb_num<mb_num;curr_mb_num++)
	{
		init_block(&block[curr_mb_num]);
		init_residu(&residu[curr_mb_num]);
		GetMbPosition(curr_mb_num,picWidthInMbs,&iMb_x,&iMb_y);
		GetBaseMbAddr(curr_nal, &block[curr_mb_num], iMb_x << 4, iMb_y << 4);
		residu[curr_mb_num].InCropWindow = is_crop_in_window(curr_sps, iMb_x, iMb_y);
		residu[curr_mb_num].MbType = GetBaseType(curr_nal, base_residu, block, iMb_x, iMb_y);
		memset(residu[curr_mb_num].LumaLevel,0,16*16);
		memset(residu[curr_mb_num].Intra16x16DCLevel,0,16);
		memset(residu[curr_mb_num].ChromaDCLevel_Cb,0,4);
		memset(residu[curr_mb_num].ChromaDCLevel_Cr,0,4);
		memset(residu[curr_mb_num].ChromaACLevel_Cb,0,4*16);
		memset(residu[curr_mb_num].ChromaACLevel_Cr,0,4*16);
		residu[curr_mb_num].BaseModeFlag = 1;
		residu[curr_mb_num].Transform8x8 = 0;
		residu[curr_mb_num].ResidualPredictionFlag = 1;
		residu[curr_mb_num].Cbp = 0;
	}

}

void base_layer_init(NAL* current_nal,int layer_id,MMO* pmmo,decoder_context* pdecoder_context,
							  int poc,int nal_ref_idc,LAYER_MMO* pcurrent_layer_mmo,
							  LIST_MMO* pcurr_pic,SPS* pcurr_sps,PPS* pcurr_pps)
{
	int pic_width_in_mbs;
	int display_layer_id = get_layer_id(pdecoder_context->dq_to_display,pdecoder_context->layer_id_table);
	DEP_MMO* pcurr_dep_mmo = pcurrent_layer_mmo->dep_mmo;
	SLICE* pcurr_slice = NULL;
	//init nal
	init_nal(current_nal,pdecoder_context);
	pcurrent_layer_mmo->nal = current_nal;
	current_nal -> b_stride = pcurr_sps -> b_stride;
	current_nal -> b8_stride = pcurr_sps -> b8_stride;
	current_nal->NalRefIdc = nal_ref_idc;
	current_nal->LayerId = layer_id;
	current_nal->DqId = pdecoder_context->layer_id_table[current_nal->LayerId];
	current_nal->DependencyId = current_nal->DqId>>4;
	current_nal->QualityId = current_nal->DqId&15;
	if (pdecoder_context->dq_to_display == current_nal->DqId)
	{
		current_nal->PicToDisplay = 1;
		current_nal->PicToDecode = 1;
	}
	else
	{
		current_nal->PicToDisplay = 0;
		current_nal->PicToDecode = pdecoder_context->UsedSpatial[current_nal->LayerId][display_layer_id];
	}
	//init slice
	pcurr_slice = current_nal->pSlice;
	pcurrent_layer_mmo->slice = pcurr_slice;
	pcurr_slice->slice_type = SLICE_TYPE_B;
	pcurr_slice->slice_num = 0;
	pic_width_in_mbs = pcurr_sps->pic_width_in_mbs;
	reinit_slice(pcurr_slice);
	pcurr_slice -> b_stride = pcurr_sps -> b_stride;
	pcurr_slice -> b8_stride = pcurr_sps -> b8_stride;
	pcurr_slice -> mb_stride = pcurr_sps -> pic_width_in_mbs;
	mapUnitToSlice(pcurr_sps, pcurr_pps, pcurrent_layer_mmo->mb_slice_group_map, pcurr_slice -> slice_group_change_cycle, pcurr_pps->slice_group_id);
	InitUcharTab(pcurr_sps -> PicSizeInMbs, pcurrent_layer_mmo->mb_slice_table);
	pcurr_slice->slice_qp_delta = pcurr_pps->pic_init_qp;
	pcurr_slice->direct_spatial_mv_pred_flag = 0;
	pcurr_slice->first_mb_in_slice = 0;
	//init pic
	pcurr_pic->memory_size = (pmmo->MaxWidth+32)*(pmmo->MaxHeight+32);
	pcurr_pic->poc = poc;
	allocate_picture_memory(pcurr_pic);
	
	

	//init others
	{
		int max_num_ref_frame = pcurr_sps->num_ref_frames>1?pcurr_sps->num_ref_frames:1;
		pcurr_dep_mmo->RefCountL0 = max_num_ref_frame;
		pcurr_dep_mmo->RefCountL1 = max_num_ref_frame;
		fill_default_ref_list(pcurr_dep_mmo,pcurr_dep_mmo->refl0,pcurr_dep_mmo->refl1,
			pcurrent_layer_mmo->currentPic,pcurr_slice->slice_type,0);
		
	}
}

void base_layer_error_conceal_key(int layer_id,MMO* pmmo,decoder_context* pdecoder_context,thread_context* pthread_context,
								  int poc,int nal_ref_idc,NAL* current_nal)
{
	//mv and ref cache
	int dqId = pdecoder_context->layer_id_table[layer_id];
	LAYER_MMO* pcurrent_layer_mmo = &(pmmo->LayerMMO[layer_id]);
	DEP_MMO* pcurr_dep_mmo = pcurrent_layer_mmo->dep_mmo;
	RESIDU* pcurrent_residu = pcurrent_layer_mmo->residu;
	int pic_parameter_id = pdecoder_context->pic_parameter_id[layer_id];
	PPS* pcurr_pps = &(pdecoder_context->all_pps[pic_parameter_id]);
	SPS *pcurr_sps = &(pdecoder_context->all_sps[pcurr_pps->seq_parameter_set_id+(layer_id == 0?0:16)]);
	SLICE* pcurr_slice = current_nal->pSlice;
	LIST_MMO* pcurr_pic = pcurrent_layer_mmo->currentPic;
	int mb_num = pcurr_sps->PicSizeInMbs;
	short iMb_x,iMb_y;
	short curr_mb_num;
	int is_svc;
	int pic_width_in_mbs;
	int mb_part_idx = 0;
	LIST_MMO* ref_pic = NULL;

	int display_layer_id = get_layer_id(pdecoder_context->dq_to_display,pdecoder_context->layer_id_table);
	DATA* block = pthread_context->Block;
	RESIDU* residu = pcurrent_layer_mmo->residu;

	pcurr_slice->slice_type = SLICE_TYPE_P;
	pic_width_in_mbs = pcurr_sps->pic_width_in_mbs;
	if (layer_id != 0)
	{
		is_svc = 1;
	}
	else
	{
		is_svc = 0;
	}
	for (curr_mb_num = 0;curr_mb_num<mb_num;curr_mb_num++)
	{
		pcurrent_layer_mmo->mb_slice_table[curr_mb_num] = pcurr_slice->slice_num;
		init_block(&block[curr_mb_num]);
		init_residu(&residu[curr_mb_num]);
		GetMbPosition(curr_mb_num,pic_width_in_mbs,&iMb_x,&iMb_y);
		residu[curr_mb_num].MbType = Pred_L0;
		memset(residu[curr_mb_num].LumaLevel,0,16*16);
		memset(residu[curr_mb_num].Intra16x16DCLevel,0,16);
		memset(residu[curr_mb_num].ChromaDCLevel_Cb,0,4);
		memset(residu[curr_mb_num].ChromaDCLevel_Cr,0,4);
		memset(residu[curr_mb_num].ChromaACLevel_Cb,0,4*16);
		memset(residu[curr_mb_num].ChromaACLevel_Cr,0,4*16);
		residu[curr_mb_num].BaseModeFlag = 0;
		residu[curr_mb_num].Transform8x8 = 0;
		residu[curr_mb_num].ResidualPredictionFlag = 0;
		residu[curr_mb_num].Cbp = 0;
	}
	
	
	direct_ref_list_init(pcurr_slice, pcurr_dep_mmo->refl0, pcurr_dep_mmo->refl1, pcurrent_layer_mmo->currentPic,
		pcurrent_layer_mmo);
	ref_pic = &(pcurr_dep_mmo->refl0[pcurr_dep_mmo->RefCountL0-1]);
	direct_dist_scale_factor(pcurr_slice -> dist_scale_factor, pcurrent_layer_mmo, pcurr_dep_mmo->refl0,  pcurr_dep_mmo->refl1,
		pcurrent_layer_mmo->currentPic -> poc);
	//temporal prediction
	pic_error_conceal_mp(current_nal,ref_pic->mv_memory_l0,ref_pic->ref_memory_l0,
		pcurr_pic->mv_memory_l0,pcurr_pic->ref_memory_l0,mb_num,pcurr_sps->pic_width_in_mbs);
	Decode_P_avc(pthread_context->current_sps_id, pthread_context->current_pps_id, current_nal->pSlice, pcurrent_layer_mmo->mb_slice_group_map, 
		pcurrent_layer_mmo->residu, pdecoder_context->pred_function->baseline_vectors, pcurr_dep_mmo->refl0, pcurrent_layer_mmo->currentPic, pthread_context->current_quantif, current_nal);
	FinishFrameSVC(current_nal,pcurr_sps,pcurr_pps,pcurrent_layer_mmo->currentPic,pcurr_slice,pthread_context->end_of_slice[0],
		pthread_context->Block,pcurrent_residu,pmmo);
	pcurrent_layer_mmo->error_concealment_done = 1;

	if (!pcurr_dep_mmo->receive_one_layer)
	{
		LIST_MMO* ec_curr_pic = (LIST_MMO*)malloc(sizeof(LIST_MMO));
		list_wrap* plist_wrap = (list_wrap*) malloc(sizeof(list_wrap));
		plist_wrap->wrap_con = ec_curr_pic;
		memcpy(ec_curr_pic,pcurrent_layer_mmo->currentPic,sizeof(LIST_MMO));
		add_list_wrap(pcurrent_layer_mmo->eclist,plist_wrap);
	}
}

void base_layer_error_conceal_non_key(int layer_id,MMO* pmmo,decoder_context* pdecoder_context,thread_context* pthread_context,
									  int poc,int nal_ref_idc,NAL* current_nal)
{
	//mv and ref cache
	short mv_cache_l0[48][2];
	short mv_cache_l1[48][2];
	short ref_cache_l0[48];
	short ref_cache_l1[48];
	int dqId = pdecoder_context->layer_id_table[layer_id];
	LAYER_MMO* pcurrent_layer_mmo = &(pmmo->LayerMMO[layer_id]);
	DEP_MMO* pcurr_dep_mmo = pcurrent_layer_mmo->dep_mmo;
	RESIDU* pcurrent_residu = pcurrent_layer_mmo->residu;
	int pic_parameter_id = pdecoder_context->pic_parameter_id[layer_id];
	PPS* pcurr_pps = &(pdecoder_context->all_pps[pic_parameter_id]);
	SPS *pcurr_sps = &(pdecoder_context->all_sps[pcurr_pps->seq_parameter_set_id+(layer_id == 0?0:16)]);
	SLICE* pcurr_slice = current_nal->pSlice;
	LIST_MMO* pcurr_pic = pcurrent_layer_mmo->currentPic;
	int mb_num = pcurr_sps->PicSizeInMbs;
	short iMb_x,iMb_y;
	short curr_mb_num;
	int is_svc;
	int pic_width_in_mbs;
	int mb_part_idx = 0;

	short* curr_mvl0;
	short* curr_mvl1;
	short* curr_refl0;
	short* curr_refl1;
	short* mv_l1_l0;
	short* mv_l1_l1;
	short* ref_l1_l0;
	short* ref_l1_l1;
	int display_layer_id = get_layer_id(pdecoder_context->dq_to_display,pdecoder_context->layer_id_table);
	DATA* block = pthread_context->Block;
	RESIDU* residu = pcurrent_layer_mmo->residu;

	pic_width_in_mbs = pcurr_sps->pic_width_in_mbs;
	curr_mvl0 = pcurr_pic->mv_memory_l0;
	curr_mvl1 = pcurr_pic->mv_memory_l1;
	curr_refl0 = pcurr_pic->ref_memory_l0;
	curr_refl1 = pcurr_pic->ref_memory_l1;
	mv_l1_l0 = pcurr_dep_mmo->refl1[0].mv_memory_l0;
	mv_l1_l1 = pcurr_dep_mmo->refl1[0].mv_memory_l1;
	ref_l1_l0 = pcurr_dep_mmo->refl1[0].ref_memory_l0;
	ref_l1_l1 = pcurr_dep_mmo->refl1[0].ref_memory_l1;
	if (layer_id != 0)
	{
		is_svc = 1;
	}
	else
	{
		is_svc = 0;
	}
	for (curr_mb_num = 0;curr_mb_num<mb_num;curr_mb_num++)
	{
		pcurrent_layer_mmo->mb_slice_table[curr_mb_num] = pcurr_slice->slice_num;
		init_block(&block[curr_mb_num]);
		init_residu(&residu[curr_mb_num]);
		GetMbPosition(curr_mb_num,pic_width_in_mbs,&iMb_x,&iMb_y);
		residu[curr_mb_num].MbType = B_Bi;
		memset(residu[curr_mb_num].LumaLevel,0,16*16);
		memset(residu[curr_mb_num].Intra16x16DCLevel,0,16);
		memset(residu[curr_mb_num].ChromaDCLevel_Cb,0,4);
		memset(residu[curr_mb_num].ChromaDCLevel_Cr,0,4);
		memset(residu[curr_mb_num].ChromaACLevel_Cb,0,4*16);
		memset(residu[curr_mb_num].ChromaACLevel_Cr,0,4*16);
		residu[curr_mb_num].BaseModeFlag = 0;
		residu[curr_mb_num].Transform8x8 = 0;
		residu[curr_mb_num].ResidualPredictionFlag = 0;
		residu[curr_mb_num].Cbp = 0;
	}
	
	direct_ref_list_init(pcurr_slice, pcurr_dep_mmo->refl0, pcurr_dep_mmo->refl1, pcurrent_layer_mmo->currentPic,
		pcurrent_layer_mmo);
	direct_dist_scale_factor(pcurr_slice -> dist_scale_factor, pcurrent_layer_mmo, pcurr_dep_mmo->refl0,  pcurr_dep_mmo->refl1,
		pcurrent_layer_mmo->currentPic -> poc);
	//temporal prediction
	for (curr_mb_num = 0;curr_mb_num<mb_num;curr_mb_num++)
	{
		GetMbPosition(curr_mb_num, pic_width_in_mbs, &iMb_x, &iMb_y);
		init_ref_cache(ref_cache_l0);
		init_ref_cache(ref_cache_l1);
		init_mv_cache(mv_cache_l0);
		init_mv_cache(mv_cache_l1);

		fill_caches_motion_vector_B_full_ref(pcurr_slice, pcurr_slice -> b_stride, pcurr_slice -> b8_stride, 
			ref_cache_l0, ref_cache_l1, mv_cache_l0, mv_cache_l1, pcurrent_layer_mmo->mb_slice_table , 
			pcurrent_layer_mmo->residu,
			&curr_mvl0 [(iMb_x << 3) + iMb_y * (pcurr_slice -> b_stride << 2)],
			&curr_mvl1 [(iMb_x << 3) + iMb_y * (pcurr_slice -> b_stride << 2)],
			&curr_refl0[(iMb_x << 2) + iMb_y * (pcurr_slice -> b_stride << 1)],
			&curr_refl1[(iMb_x << 2) + iMb_y * (pcurr_slice -> b_stride << 1)], curr_mb_num, iMb_x);
		for (mb_part_idx = 0;mb_part_idx<4;mb_part_idx++)
		{
			temporal_direct_prediction(mv_cache_l0, mv_cache_l1, mv_l1_l0, 
				ref_cache_l0, ref_cache_l1, ref_l1_l0, ref_l1_l1, pcurr_slice, 
				mb_part_idx, pcurr_sps->direct_8x8_inference_flag, is_svc, pcurr_slice->slice_type);
		}
		write_back_motion_full_ref( pcurr_slice -> b_stride, pcurr_slice -> b8_stride,
			&curr_mvl0[(iMb_x << 3) + iMb_y * (pcurr_slice -> b_stride << 2)], mv_cache_l0,
			&curr_refl0[(iMb_x << 2) + iMb_y * (pcurr_slice -> b_stride << 1)], ref_cache_l0 );
		write_back_motion_full_ref( pcurr_slice -> b_stride, pcurr_slice -> b8_stride,
			&curr_mvl1[(iMb_x << 3) + iMb_y * (pcurr_slice -> b_stride << 2)], mv_cache_l1,
			&curr_refl1[(iMb_x << 2) + iMb_y * (pcurr_slice -> b_stride << 1)], ref_cache_l1 );
	}
	Decode_B_avc(pthread_context->current_sps_id, pthread_context->current_pps_id, current_nal->pSlice, pcurrent_layer_mmo->mb_slice_group_map, 
		pcurrent_layer_mmo->residu, pdecoder_context->pred_function, pcurr_dep_mmo->refl0, 
		pcurr_dep_mmo->refl1, pcurrent_layer_mmo->currentPic, pthread_context->current_quantif, current_nal);
	FinishFrameSVC(current_nal,pcurr_sps,pcurr_pps,pcurrent_layer_mmo->currentPic,pcurr_slice,pthread_context->end_of_slice[0],
		pthread_context->Block,pcurrent_residu,pmmo);
	pcurrent_layer_mmo->error_concealment_done = 1;
	if (!pcurr_dep_mmo->receive_one_layer)
	{
		LIST_MMO* ec_curr_pic = (LIST_MMO*)malloc(sizeof(LIST_MMO));
		list_wrap* plist_wrap = (list_wrap*) malloc(sizeof(list_wrap));
		plist_wrap->wrap_con = ec_curr_pic;
		memcpy(ec_curr_pic,pcurrent_layer_mmo->currentPic,sizeof(LIST_MMO));
		add_list_wrap(pcurrent_layer_mmo->eclist,plist_wrap);
	}

}
void enhance_layer_error_conceal(int layerId,MMO* pmmo,decoder_context* pdecoder_context,thread_context* pthread_context,int poc)
{
	int dqId = pdecoder_context->layer_id_table[layerId];
	LAYER_MMO* pcurrent_layer_mmo = &(pmmo->LayerMMO[layerId]);
	LAYER_MMO* pbase_layer_mmo = &(pmmo->LayerMMO[layerId-1]);
	DEP_MMO* pcurr_dep_mmo = pcurrent_layer_mmo->dep_mmo;
	RESIDU* pcurrent_residu = pcurrent_layer_mmo->residu;
	RESIDU* pbase_residu = pbase_layer_mmo->residu;
	int pic_parameter_id = pdecoder_context->pic_parameter_id[layerId];
	PPS* pcurr_pps = &(pdecoder_context->all_pps[pic_parameter_id]);
	SPS *pcurr_sps = &(pdecoder_context->all_sps[pcurr_pps->seq_parameter_set_id+16]);
	int base_pps_id = pdecoder_context->pic_parameter_id[layerId-1];
	PPS* pbase_pps = &(pdecoder_context->all_pps[base_pps_id]);
	int delta_sps_id = (layerId>1)?16:0;
	SPS* pbase_sps = &(pdecoder_context->all_sps[pbase_pps->seq_parameter_set_id+delta_sps_id]);
	NAL* current_nal = (NAL*)malloc(sizeof(NAL));
	SLICE* pcurr_slice = NULL;
	int mb_num = pcurr_sps->PicSizeInMbs;
	int base_mb_num = pbase_sps->PicSizeInMbs;
	int curr_mb_num = 0;
	SLICE* pbase_slice;
	NAL* base_nal;
	int display_layer_id = get_layer_id(pdecoder_context->dq_to_display,pdecoder_context->layer_id_table);
	

	//check for lower layer
	if (!pbase_layer_mmo->receive_new_nal && !pbase_layer_mmo->error_concealment_done)
	{
		enhance_layer_error_conceal(layerId-1,pmmo,pdecoder_context,pthread_context,poc);
	}

	//init data
	base_nal = pbase_layer_mmo->nal;
	pbase_slice = pbase_layer_mmo->slice;
	init_nal(current_nal,pdecoder_context);
	reinit_slice(current_nal->pSlice);
	pcurr_slice = current_nal->pSlice;
	pcurr_slice->first_mb_in_slice = 0;
	pcurr_slice->slice_type = pbase_slice->slice_type;
	pcurr_slice->slice_qp_delta = pcurr_pps->pic_init_qp;
	pcurr_slice -> mb_stride = pcurr_sps -> pic_width_in_mbs;
	current_nal -> b_stride = pcurr_sps -> b_stride;
	current_nal -> b8_stride = pcurr_sps -> b8_stride;
	current_nal->NalRefIdc = base_nal->NalRefIdc;
	current_nal->LayerId = layerId;
	current_nal->DqId = pdecoder_context->layer_id_table[current_nal->LayerId];
	current_nal->DependencyId = current_nal->DqId>>4;
	current_nal->QualityId = current_nal->DqId&15;
	pcurr_dep_mmo->active = 1;
	current_nal->bFinishLayerRepresent = 1;
	current_nal->BaseLayerId = current_nal->LayerId-1;
	NalParamInit(current_nal,pdecoder_context->all_pps,pdecoder_context->all_sps,pcurr_sps);
	pcurrent_layer_mmo->slice = current_nal->pSlice;
	pcurrent_layer_mmo->nal = current_nal;
	current_nal->TCoeffPrediction = 0;
	current_nal->SCoeffPrediction = !current_nal->SpatialScalability;
	
	pthread_context->end_of_slice[0] = 0;
	pcurrent_layer_mmo->currentPic->poc = poc;

	if (pcurr_dep_mmo->receive_one_layer)
	{
		current_nal->NalRefIdc = pcurr_dep_mmo->nal_ref_idc;
		pcurrent_layer_mmo->currentPic->pic_num = pcurr_dep_mmo->frame_num;
		if (current_nal->NalRefIdc>0)
		{
			pcurrent_layer_mmo->currentPic->reference = 3;
		}
		else
		{
			pcurrent_layer_mmo->currentPic->reference = 0;
		}
	}
	printf("error concealment poc:%d\t layer id:%d\n",pcurrent_layer_mmo->currentPic->poc,
		layerId);
	if(SLICE_TYPE_I != pcurr_slice->slice_type && current_nal->QualityId == 0)
	{
		int max_num_ref_frame = pcurr_sps->num_ref_frames>1?pcurr_sps->num_ref_frames:1;
		pcurr_dep_mmo->RefCountL0 = max_num_ref_frame;
		pcurr_dep_mmo->RefCountL1 = max_num_ref_frame;
		fill_default_ref_list(pcurr_dep_mmo,pcurr_dep_mmo->refl0,pcurr_dep_mmo->refl1,
			pcurrent_layer_mmo->currentPic,pcurr_slice->slice_type,0);
	}
	direct_ref_list_init(pcurr_slice, pcurr_dep_mmo->refl0, pcurr_dep_mmo->refl1, pcurrent_layer_mmo->currentPic,
		pcurrent_layer_mmo);
	error_conceal_init_slice(current_nal,pcurr_slice,pthread_context->Block,pcurrent_layer_mmo->residu,
		pbase_layer_mmo->residu,pcurr_sps->pic_width_in_mbs,mb_num,pcurr_sps);

	if (layerId == display_layer_id)
	{
		current_nal->PicToDecode = 1;
		current_nal->PicToDisplay = 1;
	}
	else
	{
		current_nal->PicToDecode = pdecoder_context->UsedSpatial[layerId][display_layer_id];
		current_nal->PicToDisplay = 0;
	}
	current_nal->NalToSkip = 0;
	current_nal->base_picture = pbase_layer_mmo->currentPic;
	current_nal->current_picture = pcurrent_layer_mmo->currentPic;
	pcurrent_layer_mmo->currentPic->memory_size = (pmmo->MaxWidth+32)*(pmmo->MaxHeight+32);
	pcurrent_layer_mmo->currentPic->pic_width = pcurr_sps->pic_width_in_mbs*16;
	pcurrent_layer_mmo->currentPic->pic_height = pcurr_sps->pic_height_in_map_units*16;
	allocate_picture_memory(pcurrent_layer_mmo->currentPic);
	if (!pcurr_dep_mmo->receive_one_layer)
	{
		LIST_MMO* ec_curr_pic = (LIST_MMO*)malloc(sizeof(LIST_MMO));
		list_wrap* plist_wrap = (list_wrap*) malloc(sizeof(list_wrap));
		plist_wrap->wrap_con = ec_curr_pic;
		memcpy(ec_curr_pic,pcurrent_layer_mmo->currentPic,sizeof(LIST_MMO));
		add_list_wrap(pcurrent_layer_mmo->eclist,plist_wrap);
	}
	if (0 == (dqId&15))
	{
		pcurrent_layer_mmo->dep_mmo->store_ref_base_pic_flag = 0;
		pcurrent_layer_mmo->dep_mmo->idr_flag = 0;
	}
	switch (pcurr_slice->slice_type)
	{
	case 0 : {/* case_55 */
		Decode_P_svc(MB_NUM, pdecoder_context->all_sps, pdecoder_context->all_pps, pcurr_slice, 
			current_nal, pcurrent_layer_mmo->mb_slice_table, pthread_context->Block,pcurrent_residu, 
			pdecoder_context->pred_function->baseline_vectors, pcurr_dep_mmo->refl0,
			pcurrent_layer_mmo->currentPic, pthread_context->current_quantif, pdecoder_context->svc_function,
			pthread_context->svc_px, pthread_context->svc_py, pthread_context->svc_Upsampling_tmp,
			pthread_context->svc_xk16,pthread_context->svc_xp16, pthread_context->svc_yk16, 
			pthread_context->svc_yp16,pdecoder_context->mmo);
		break; }/* case_55 */
	case 1 : {/* case_56 */
		Decode_B_svc(MB_NUM, pdecoder_context->all_sps, pdecoder_context->all_pps, pcurr_slice, 
			current_nal,  pthread_context->Block, pcurrent_residu, 
			pdecoder_context->pred_function, pcurr_dep_mmo->refl0,pcurr_dep_mmo->refl1,
			pcurrent_layer_mmo->currentPic, pthread_context->current_quantif, pdecoder_context->svc_function, 
			pthread_context->svc_px, pthread_context->svc_py, pthread_context->svc_Upsampling_tmp,
			pthread_context->svc_xk16,pthread_context->svc_xp16, pthread_context->svc_yk16, 
			pthread_context->svc_yp16,pdecoder_context->mmo);
		break; }/* case_56 */
	case 2 : {/* case_57 */
		Decode_I_svc(MB_NUM, pdecoder_context->all_sps, pdecoder_context->all_pps, pcurr_slice,
			current_nal,  pthread_context->Block, pcurrent_residu, 
			pdecoder_context->pred_function->baseline_vectors, pcurrent_layer_mmo->currentPic,
			pthread_context->current_quantif,pdecoder_context->mmo);
		break; }
	}
	FinishFrameSVC(current_nal,pcurr_sps,pcurr_pps,pcurrent_layer_mmo->currentPic,pcurr_slice,pthread_context->end_of_slice[0],
		pthread_context->Block,pcurrent_residu,pmmo);
	pcurrent_layer_mmo->error_concealment_done = 1;
}

void base_layer_error_conceal(int layer_id,MMO* pmmo,decoder_context* pdecoder_context,thread_context* pthread_context,
							  int poc,int nal_ref_idc)
{
	int dqId = pdecoder_context->layer_id_table[layer_id];
	LAYER_MMO* pcurrent_layer_mmo = &(pmmo->LayerMMO[layer_id]);
	DEP_MMO* pcurr_dep_mmo = pcurrent_layer_mmo->dep_mmo;
	RESIDU* pcurrent_residu = pcurrent_layer_mmo->residu;
	int pic_parameter_id = pdecoder_context->pic_parameter_id[layer_id];
	PPS* pcurr_pps = &(pdecoder_context->all_pps[pic_parameter_id]);
	SPS *pcurr_sps = &(pdecoder_context->all_sps[pcurr_pps->seq_parameter_set_id+(layer_id == 0?0:16)]);
	NAL* current_nal = (NAL*)malloc(sizeof(NAL));
	SLICE* pcurr_slice = NULL;
	LIST_MMO* pcurr_pic = pcurrent_layer_mmo->currentPic;
	base_layer_init(current_nal,layer_id,pmmo,pdecoder_context,poc,nal_ref_idc,pcurrent_layer_mmo,pcurr_pic,
		pcurr_sps,pcurr_pps);
	//if (pcurr_dep_mmo->refl1[0].poc < poc)
	if (1)
	{
		pdecoder_context->base_layer_lost = 1;
		base_layer_error_conceal_key(layer_id,pmmo,pdecoder_context,pthread_context,poc,nal_ref_idc,current_nal);
	}
	else
	{
		pdecoder_context->base_layer_lost = 2;
		base_layer_error_conceal_non_key(layer_id,pmmo,pdecoder_context,pthread_context,poc,nal_ref_idc,current_nal);
	}
}