#include "decode_base_layer_nal.h"
#include "slice_header_svc.h"
#include "slice_data_cabac_svc.h"
#include "slice_data_cavlc.h"
#include "Slice_data_avc.h"
#include "FinishFrameSVC.h"
#include "MemoryManagement.h"
#include "nal_unit_header_svc_extension.h"
#include "cal_time.h"

extern double layer_time[8][2];
void base_layer_parse(NAL *pNal,decoder_context* pdecoder_context,
					  thread_context* pthread_context)
{
	LAYER_MMO* pLayerMmo;
	DEP_MMO* pdep_mmo;
	pLayerMmo = &(pdecoder_context->mmo->LayerMMO[pNal->LayerId]);
	pdep_mmo = pLayerMmo->dep_mmo;
	slice_header_svc( pNal->prbsp_memory, pthread_context->current_sps_id, 
		pthread_context->current_pps_id, pthread_context->entropy_flag,
		pthread_context->current_quantif, pLayerMmo->currentPic, pdecoder_context->all_sps, 
		pdecoder_context->all_pps, pNal->current_position, pNal->pSlice,
		pdecoder_context->mmo, pdep_mmo->refl0, pdep_mmo->refl1, 
		pNal, pthread_context->end_of_slice,pdecoder_context->dq_to_display,
		pdecoder_context,pthread_context);
	SetBaseNalToDisplay(pNal, *(pthread_context->end_of_slice),pdecoder_context);
	if (0 == pNal->QualityId)
	{
		DEP_MMO* dep_mmo = pLayerMmo->dep_mmo;
		dep_mmo->idr_flag = pNal->IdrFlag;
		dep_mmo->store_ref_base_pic_flag = pNal->StoreRefBasePicFlag;
		dep_mmo->nal_ref_idc = (char)pNal->NalRefIdc;
	}
	
	switch (pthread_context->entropy_flag[0]) { /* switch_18 */
	  case 0 : {/* case_19 */
		  slice_base_layer_cavlc(pNal->prbsp_memory, pNal->nal_byte_size, 
			  pNal->current_position, pthread_context->current_sps_id, pthread_context->current_pps_id, 
			  pdecoder_context->vlc_table,pLayerMmo->currentPic, pdep_mmo->refl1, 
			  pNal,pNal->pSlice, pthread_context->Block,
			  pLayerMmo->residu, pthread_context->end_of_slice,pdecoder_context->mmo);
		  break; }/* case_19 */
	  case 1 : {/* case_20 */
		  slice_base_layer_cabac(pNal->prbsp_memory, pNal->nal_byte_size, 
			  pNal->current_position, pthread_context->current_sps_id, pthread_context->current_pps_id, 
			  pLayerMmo->currentPic, 
			  pdep_mmo->refl1, pNal, pthread_context->mv_cabac_l0_o,
			  pthread_context->mv_cabac_l1_o, pthread_context->ref_cabac_l0_o, 
			  pthread_context->ref_cabac_l1_o, 
			  pNal->pSlice,pthread_context->Block, pLayerMmo->residu,
			  pthread_context->end_of_slice,pdecoder_context->mmo);
		  break; }
	}
	
}

void decode_base_layer_nal(NAL* pNal,decoder_context* pdecoder_context,
						   thread_context* pthread_context,display_context* pdisplay_con)
{
	LAYER_MMO* pLayerMmo;
	DEP_MMO* pdep_mmo;
#ifdef _CAL_TIME_
	time_start();
#endif
	base_layer_parse(pNal,pdecoder_context,pthread_context);
#ifdef _CAL_TIME_
	layer_time[pNal->LayerId][0]+=time_end();
	time_start();
#endif
	pLayerMmo = &(pdecoder_context->mmo->LayerMMO[pNal->LayerId]);
	pdep_mmo = pLayerMmo->dep_mmo;
	switch (pNal->pSlice->slice_type) { /* switch_36 */
	  case 0 : {
		  Decode_P_avc(pthread_context->current_sps_id, pthread_context->current_pps_id, 
			  pNal->pSlice, pLayerMmo->mb_slice_group_map, 
			  pLayerMmo->residu, pdecoder_context->pred_function->baseline_vectors, 
			  pdep_mmo->refl0, pLayerMmo->currentPic, pthread_context->current_quantif, pNal);
		  break; }/* case_37 */
	  case 1 : {
		  Decode_B_avc(pthread_context->current_sps_id, pthread_context->current_pps_id, pNal->pSlice, pLayerMmo->mb_slice_group_map, 
			  pLayerMmo->residu, pdecoder_context->pred_function, pdep_mmo->refl0, 
			  pdep_mmo->refl1, pLayerMmo->currentPic, pthread_context->current_quantif, pNal);
		  break; }/* case_38 */
	  case 2 : {
		  Decode_I_avc(pthread_context->current_sps_id, pthread_context->current_pps_id, pNal->pSlice, pLayerMmo->mb_slice_group_map,
			  pLayerMmo->residu, pdecoder_context->pred_function->baseline_vectors, 
			  pthread_context->current_quantif, pNal);
		  break; }/* case_39 */
	} /* end switch_36 */
	FinishFrameSVC(pNal, pthread_context->current_sps_id, pthread_context->current_pps_id, 
		pLayerMmo->currentPic, 
		pNal->pSlice, pthread_context->end_of_slice[0], 
		pthread_context->Block, pLayerMmo->residu,pdecoder_context->mmo);
#ifdef _CAL_TIME_
	layer_time[pNal->LayerId][1]+=time_end();
#endif
}

void decode_base_layer_idr_nal(NAL* pNal,decoder_context* pdecoder_context,
							   thread_context* pthread_context,display_context* pdisplay_con)
{
	LAYER_MMO* pLayerMmo;
	svc_calculate_dpb(max_layer, pdecoder_context->mmo, pdecoder_context->all_sps);
	pLayerMmo = &(pdecoder_context->mmo->LayerMMO[pNal->LayerId]);
#ifdef _CAL_TIME_
	time_start();
#endif
	base_layer_parse(pNal,pdecoder_context,pthread_context);
#ifdef _CAL_TIME_
	layer_time[pNal->LayerId][0] += time_end();
	time_start();
#endif
	Decode_I_avc(pthread_context->current_sps_id, pthread_context->current_pps_id, pNal->pSlice, 
		pLayerMmo->mb_slice_group_map, pLayerMmo->residu, 
		pdecoder_context->pred_function->baseline_vectors, 
		pthread_context->current_quantif, pNal);
	FinishFrameSVC( pNal, pthread_context->current_sps_id, pthread_context->current_pps_id,
		pLayerMmo->currentPic,
		pNal->pSlice, pthread_context->end_of_slice[0], 
		pthread_context->Block, pLayerMmo->residu,pdecoder_context->mmo);
#ifdef _CAL_TIME_
	layer_time[pNal->LayerId][1] += time_end();
#endif
}