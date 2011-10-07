

#include "type.h"
#include "symbol.h"
#include "svc_data.h"
#include "svc_type.h"
#include "main_data.h"
#include "vlc_cabac.h"
#include "cabac_svc.h"
#include "VlcCabacSvc.h"
#include "CabacIMacroBlock.h"
#include "fill_caches_cabac.h"
#include "CabacIMacroBlock.h"
#include "macroblock_partitionning.h"
#include "fill_caches_svc.h"



int CabacIPartioning (const NAL * Nal,RESIDU * base_picture_residu,RESIDU *picture_residu, SLICE *slice, DATA Tab_block[],	DATA *block, 
							unsigned char non_zero_count_cache [], unsigned char slice_table [],
							int iCurrMbAddr , CABACContext *cabac, unsigned char *cabac_state,
							CABAC_NEIGH *neigh,const short x,const short y, int constrained_intra_pred_flag,
							int dct_allowed, int *last_QP, int *mb_qp_delta)
{
	int    coded_block_pattern ;
	short intra4x4_pred_mode_cache[40];

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckIMbType(picture_residu -> MbType)){
		return 1;
	}
#endif

	//Updating the slice table in order to save in which slice each macroblock belong to
	if ( picture_residu -> MbType == INTRA_PCM ) {
		unsigned char *ptr = cabac -> bytestream;
		int Position = 0;
		if (cabac -> low & 0x1) {
			ptr -= CABAC_BITS >> 3;
		}
		ParseIPCM(ptr, &Position, picture_residu, non_zero_count_cache);
		init_cabac_decoder(cabac, &ptr[384], (int)(cabac -> bytestream_end - ptr));
		picture_residu -> Cbp = 0x1ef;

	} else {
		//Updating the parameter in order to decode the VLC
		fill_caches_svc(slice,picture_residu,base_picture_residu,1,non_zero_count_cache,slice_table,block,intra4x4_pred_mode_cache,
			x,y,constrained_intra_pred_flag,Nal->TCoeffPrediction);
		fill_caches_cabac (slice, neigh, slice_table, iCurrMbAddr, *mb_qp_delta, picture_residu );

		//Recovery of the motion vectors for the sub_macroblock 
		if ( dct_allowed && picture_residu -> MbType == INTRA_4x4 && 
			decode_cabac_mb_transform_size(cabac, cabac_state, Tab_block, picture_residu -> AvailMask, iCurrMbAddr, slice -> mb_stride)){
				picture_residu -> MbType = picture_residu -> Transform8x8 = block -> Transform8x8 = block -> MbPartPredMode [0] = INTRA_8x8;
		}

		//Recovery of the prediction mode and the motion vectors for the macroblock 
		if ( block -> MbPartPredMode [0] == INTRA_4x4){
			mb_pred_intra_cabac(cabac, cabac_state, picture_residu -> Intra16x16DCLevel, intra4x4_pred_mode_cache);
			picture_residu -> IntraChromaPredMode = decode_cabac_mb_chroma_pre_mode( cabac, cabac_state, picture_residu -> AvailMask, neigh);
		}else if (block -> Transform8x8){
			mb_pred_intra_8x8_cabac(cabac, cabac_state, picture_residu -> Intra16x16DCLevel, intra4x4_pred_mode_cache);
			picture_residu -> IntraChromaPredMode = decode_cabac_mb_chroma_pre_mode( cabac, cabac_state, picture_residu -> AvailMask, neigh);
		}else if ( !IS_BL(block -> MbPartPredMode [0])){
			picture_residu -> IntraChromaPredMode = decode_cabac_mb_chroma_pre_mode( cabac, cabac_state, picture_residu -> AvailMask,  neigh);
		}

#ifdef ERROR_DETECTION
		//Error detection
		if(ErrorsCheckIntraChromaPredMode(&picture_residu -> IntraChromaPredMode)){
			return 1;
		}
#endif


		if ( block -> MbPartPredMode[0] != INTRA_16x16) {
			picture_residu -> Cbp = decode_cabac_mb_cbp_luma( cabac, cabac_state, picture_residu -> AvailMask, neigh, x, y) ;
			picture_residu -> Cbp |= (decode_cabac_mb_cbp_chroma( cabac, cabac_state, neigh)) << 4;
		}

		coded_block_pattern = picture_residu -> Cbp;

		if ( picture_residu -> Cbp > 0 || block -> MbPartPredMode[0] == INTRA_16x16 ) {
			if (picture_residu -> Cbp & 0x0f && dct_allowed && IS_BL(picture_residu -> MbType) && 
				decode_cabac_mb_transform_size(cabac, cabac_state, Tab_block, picture_residu -> AvailMask, iCurrMbAddr, slice -> mb_stride)){
					picture_residu -> Transform8x8 = block -> Transform8x8 = block -> MbPartPredMode [0] = INTRA_8x8;
			}
			*mb_qp_delta = decode_cabac_mb_dqp( cabac, cabac_state, neigh, picture_residu[- 1] . MbType);
#ifdef TI_OPTIM
			//Initialization of the macroblock neighbourhood
			*last_QP = picture_residu -> Qp = divide(*last_QP + *mb_qp_delta + 52, 52) >> 8;
#else
			*last_QP = picture_residu -> Qp = (*last_QP + *mb_qp_delta + 52) % 52;
#endif

			//Decoding process of the VLC 
			residual_cabac(cabac, cabac_state, picture_residu, neigh, non_zero_count_cache, &coded_block_pattern);
			picture_residu -> Cbp = coded_block_pattern;

		} else {
			//In case of there is no error in the prediction
			init_mb_skip(non_zero_count_cache);
			picture_residu -> Qp = *last_QP;
		}
	}
	return 0;
}
