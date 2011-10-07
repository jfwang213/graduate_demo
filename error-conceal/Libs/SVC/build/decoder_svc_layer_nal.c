#include "decode_svc_layer_nal.h"
#include "nal_unit_header_svc_extension.h"
#include "slice_header_svc.h"
#include "SliceHeaderAnnex.h"
#include "SVCDisplay.h"
#include "slice_layer_in_scalable_extension_rbsp.h"
#include "slice_data_P_svc.h"
#include "slice_data_B_svc.h"
#include "slice_data_I_svc.h"
#include "decoder_context.h"
#include "FinishFrameSVC.h"

#include "cal_time.h"
#include "stdio.h"
#include "ErrorConcealment.h"
#include "layer_utils.h"
#include "pic_list.h"

extern double layer_time[8][2];

void decode_svc_layer_nal(NAL *pNal,decoder_context* pdecoder_context,thread_context *pthread_context,
						  display_context *pdisplay_context)
{
	LAYER_MMO* pLayerMmo;
	DEP_MMO* pdep_mmo;
#ifdef _CAL_TIME_
	time_start();
#endif
	NalUnitHeader(pNal->prbsp_memory, pNal->current_position, pthread_context->layer_useful, 
		pNal, pthread_context->end_of_slice);
	pLayerMmo = &(pdecoder_context->mmo->LayerMMO[pNal->LayerId]);
	pdep_mmo = pLayerMmo->dep_mmo;
	if (0 == pthread_context->layer_useful[0])
	{
		slice_header_svc(pNal->prbsp_memory, pthread_context->current_sps_id, 
			pthread_context->current_pps_id, pthread_context->entropy_flag, 
			pthread_context->current_quantif, pLayerMmo->currentPic, pdecoder_context->all_sps, pdecoder_context->all_pps, 
			pNal->current_position, pNal->pSlice, pdecoder_context->mmo, pdep_mmo->refl0, 
			pdep_mmo->refl1, pNal, pthread_context->end_of_slice,pdecoder_context->dq_to_display,
			pdecoder_context,pthread_context);
		//check for inter layer exist
		if (!pNal->no_inter_layer_pred_flag)
		{
			if (!pdecoder_context->mmo->LayerMMO[pNal->BaseLayerId].receive_new_nal)
			{
				int base_layer_id = get_layer_id(pNal->DqId-pNal->QualityId,pdecoder_context->layer_id_table);
				int t_layer;
				char is_lowest_layer = 1;
				printf("detect one lower layer miss\n");
				for (t_layer = 0;t_layer<pNal->BaseLayerId;t_layer++)
				{
					if (pdecoder_context->mmo->LayerMMO[t_layer].receive_new_nal)
					{
						is_lowest_layer = 0;
						break;
					}
				}
				if (is_lowest_layer)
				{
					base_layer_error_conceal(pNal->BaseLayerId,pdecoder_context->mmo,pdecoder_context,
						pthread_context,pLayerMmo->currentPic->poc,pdep_mmo->nal_ref_idc);
				}
				else
				{
					enhance_layer_error_conceal(pNal->BaseLayerId,pdecoder_context->mmo,pdecoder_context,pthread_context,pLayerMmo->currentPic->poc);
				}
				//recover refer
				if (!(pdecoder_context->mmo->LayerMMO[base_layer_id].receive_new_nal))
				{
					fill_default_ref_list(pdep_mmo,pdep_mmo->refl0,pdep_mmo->refl1,
						pLayerMmo->currentPic,pLayerMmo->slice->slice_type,0);
				}
			}
		}
		if (0 == pNal->QualityId)
		{
			DEP_MMO* dep_mmo = pLayerMmo->dep_mmo;
			dep_mmo->store_ref_base_pic_flag = pNal->StoreRefBasePicFlag;
		}
		pLayerMmo->dep_mmo->idr_flag = pNal->IdrFlag;
		pLayerMmo->dep_mmo->nal_ref_idc = (char)pNal->NalRefIdc;
		CheckPicToDisplay(pNal, *(pthread_context->end_of_slice),pdecoder_context);
		if (!pthread_context->entropy_flag[0])
		{
			slice_data_in_scalable_extension_cavlc(MB_NUM, pNal->prbsp_memory, pNal->nal_byte_size, pNal->current_position,
				pNal, pthread_context->current_sps_id, pthread_context->current_pps_id, pdecoder_context->vlc_table, 
				pNal->pSlice,pthread_context->Block, pLayerMmo->residu, 
				pthread_context->end_of_slice,pdecoder_context->mmo);
		}
		else
		{
			SliceCabac(MB_NUM, pNal->prbsp_memory, pNal->nal_byte_size, pNal->current_position,
				pNal, pthread_context->current_sps_id, pthread_context->current_pps_id, 
				pthread_context->mv_cabac_l0_o, pthread_context->mv_cabac_l1_o, 
				pthread_context->ref_cabac_l0_o,pthread_context->ref_cabac_l1_o, 
				pNal->pSlice,pthread_context->Block, pLayerMmo->residu, 
				pthread_context->end_of_slice,pdecoder_context->mmo);
		}

		
#ifdef _CAL_TIME_
		layer_time[pNal->LayerId][0] += time_end();
		time_start();
#endif
		if (SLICE_TYPE_I == pNal->pSlice->slice_type)
		{
			if (1 == pdecoder_context->base_layer_lost)
			{
				pNal->pSlice->slice_type = SLICE_TYPE_P;
			}
			else if (2 == pdecoder_context->base_layer_lost)
			{
				pNal->pSlice->slice_type = SLICE_TYPE_B;
			}
		}
		switch (pNal->pSlice->slice_type)
		{
		case 0 : {/* case_55 */
			Decode_P_svc(MB_NUM, pdecoder_context->all_sps, pdecoder_context->all_pps, pNal->pSlice, 
				pNal, pLayerMmo->mb_slice_table, pthread_context->Block, pLayerMmo->residu, 
				pdecoder_context->pred_function->baseline_vectors, pdep_mmo->refl0,
				pLayerMmo->currentPic, pthread_context->current_quantif, pdecoder_context->svc_function,
				pthread_context->svc_px, pthread_context->svc_py, pthread_context->svc_Upsampling_tmp,
				pthread_context->svc_xk16,pthread_context->svc_xp16, pthread_context->svc_yk16, 
				pthread_context->svc_yp16,pdecoder_context->mmo);
			break; }/* case_55 */
		case 1 : {/* case_56 */
			Decode_B_svc(MB_NUM, pdecoder_context->all_sps, pdecoder_context->all_pps, pNal->pSlice, 
				pNal,  pthread_context->Block, pLayerMmo->residu, 
				pdecoder_context->pred_function, pdep_mmo->refl0,pdep_mmo->refl1,
				pLayerMmo->currentPic, pthread_context->current_quantif, pdecoder_context->svc_function, 
				pthread_context->svc_px, pthread_context->svc_py, pthread_context->svc_Upsampling_tmp,
				pthread_context->svc_xk16,pthread_context->svc_xp16, pthread_context->svc_yk16, 
				pthread_context->svc_yp16,pdecoder_context->mmo);
			break; }/* case_56 */
		case 2 : {/* case_57 */
			Decode_I_svc(MB_NUM, pdecoder_context->all_sps, pdecoder_context->all_pps, pNal->pSlice,
				pNal,  pthread_context->Block, pLayerMmo->residu, 
				pdecoder_context->pred_function->baseline_vectors, pLayerMmo->currentPic,
				pthread_context->current_quantif,pdecoder_context->mmo);
			break; }
		}
		FinishFrameSVC(pNal, pthread_context->current_sps_id, pthread_context->current_pps_id, pLayerMmo->currentPic,
			pNal->pSlice, pthread_context->end_of_slice[0],
			pthread_context->Block, pLayerMmo->residu,pdecoder_context->mmo);
#ifdef _CAL_TIME_
		layer_time[pNal->LayerId][1] += time_end();
#endif
	}
	else
	{
		
	}
	
}


void decode_prefix_nal(NAL *pNal,decoder_context *pdecoder_context,
					   thread_context *pthread_context)
{
	PrefixNalUnit(pNal->prbsp_memory, pNal->nal_byte_size, pNal, 
		pdecoder_context->mmo, pdecoder_context->all_sps, pthread_context->layer_useful, 
		pthread_context->end_of_slice,pdecoder_context);
}