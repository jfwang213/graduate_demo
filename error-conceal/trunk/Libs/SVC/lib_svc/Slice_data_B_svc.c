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
* This librariMb_y is free software; iMb_you can redistribute it and/or
* modifiMb_y it under the terms of the GNU Lesser General Public
* License as published biMb_y the Free Software Foundation; either
* version 2 of the License, or (at iMb_your option) aniMb_y later version.
*
* This librariMb_y is distributed in the hope that it will be useful,
* but WITHOUT ANiMb_y WARRANTiMb_y; without even the implied warrantiMb_y of
* MERCHANTABILITiMb_y or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* iMb_you should have received a copiMb_y of the GNU Lesser General Public
* License along with this librariMb_y; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*
*
* $Id$
*
**************************************************************************/



#include <stdio.h>


//AVC Files
#include "main_data.h"
#include "init_data.h"
#include "decode_mb_I.h"
#include "neighbourhood.h"
#include "SliceGroupMap.h"



//SVC Files
#include "svc_type.h"
#include "I_BL.h"
#include "P_BL.h"
#include "B_BL.h"
#include "svc_data.h"
#include "Coordonates.h"
#include "WriteBackSVC.h"
#include "fill_caches_svc.h"
#include "ResidualProcessing.h"
#include "InterLayerPrediction.h"






void Decode_B_svc ( const int size_Mb, SPS *ai_pstSps, PPS * ai_pstPps, SLICE *Slice, NAL * Nal,  
				   const DATA ai_tstTab_Block [ ], RESIDU *curr_residu, const MAIN_STRUCT_PF *vectors, LIST_MMO *ai_pstRefPicListL0,	
				   LIST_MMO *ai_pstRefPicListL1, LIST_MMO *Current_pic, W_TABLES *quantif, SVC_VECTORS *svc_vectors, short *px, 
				   short *py, short *Upsampling_tmp, short *xk16, short *xp16, short *yk16, short *yp16,MMO *mmo)
{


	if (!Nal -> NalToSkip){
		LAYER_MMO* pLayerMmo = &(mmo->LayerMMO[Nal->LayerId]);
		__declspec(align(64)) short  ref_cache_l0 [48];
		__declspec(align(64)) short  ref_cache_l1 [48];
		__declspec(align(64)) short  mv_cache_l0 [48][2];
		__declspec(align(64)) short  mv_cache_l1 [48][2];
		short  x_mbPartIdx;
		short  y_mbPartIdx;
		short  x_base;
		short  y_base;

		int pic_parameter_id = Nal -> pic_parameter_id[Nal -> LayerId];
		const PPS * Pps = &ai_pstPps[pic_parameter_id];
		SPS * Sps = &ai_pstSps[Pps -> seq_parameter_set_id + 16];

		const short PicSizeInMbs = Sps -> PicSizeInMbs ;
		const short PicWiInMbs = Sps -> pic_width_in_mbs;
		const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;
		const short PicHeightInMbs = Sps -> pic_height_in_map_units + 2; 
		const short PicWidthInPix = (PicWidthInMbs << 4);
		const short offset_L = ( PicWidthInMbs << 8) + 16;
		const short offset_C = ( PicWidthInMbs << 6) + 8;  

		unsigned char *Y = offset_L + Current_pic->picture_memory_y;
		unsigned char *U = offset_C + Current_pic->picture_memory_u;
		unsigned char *V = offset_C + Current_pic->picture_memory_v;
		unsigned char *BaseY = Nal->base_picture->picture_memory_y + 16 + ((Nal -> base_Width + 32) << 4);
		unsigned char *BaseU = Nal->base_picture->picture_memory_u + 8 + ((Nal -> base_Width + 32) << 2);
		unsigned char *BaseV = Nal->base_picture->picture_memory_v + 8 + ((Nal -> base_Width + 32) << 2);
 

		RESIDU *base_residu = mmo->LayerMMO[Nal->BaseLayerId].residu;
		REFPART RefPartitionL0[32];

		//Parameters of the current slice
		short *mv_l0 = Current_pic->mv_memory_l0;
		short *mv_l1 = Current_pic->mv_memory_l1;
		short *ref_l0 = Current_pic->ref_memory_l0;
		short *ref_l1 = Current_pic->ref_memory_l1;

		//Parameters of the colocated slice in the base layer
		short *mv_base_l0 =Nal->base_picture->mv_memory_l0;
		short *mv_base_l1 = Nal->base_picture->mv_memory_l1;
		short *ref_base_l0 = Nal->base_picture->ref_memory_l0;
		short *ref_base_l1 = Nal->base_picture->ref_memory_l1;
		
		short *Residu_Luma = Current_pic->residual_memory_y; 
		short *Residu_Cb =Current_pic->residual_memory_u;
		short *Residu_Cr = Current_pic->residual_memory_v;

		short iCurrMbAddr = Slice -> first_mb_in_slice ;

		short iMb_x;
		short iMb_y;
		

		
		//initialize macroblock position
		GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);



		GetBasePrediction(Nal, Slice, ai_pstPps, ai_pstSps, Sps, curr_residu, base_residu, size_Mb, 
			iCurrMbAddr, pLayerMmo->mb_slice_table, px, py, xk16, xp16, yk16, yp16, Upsampling_tmp, 
			BaseY, BaseU, BaseV);
			
			


		for (; iCurrMbAddr < PicSizeInMbs ;) {
			init_ref_cache(ref_cache_l0);
			init_ref_cache(ref_cache_l1);
			init_mv_cache(mv_cache_l0);
			init_mv_cache(mv_cache_l1);

			//debug
			if (258 == iCurrMbAddr)
			{
				iCurrMbAddr = 258;
			}
			// I_PCM || INTRA_4iMb_x4 || INTRA_16iMb_x16
			if ( IS_I(curr_residu [iCurrMbAddr] . MbType) ) {	
				//Initalization of the neighbourhood
				ref_cache_l0[14] = ref_cache_l0[30] = -1;
				ref_cache_l1[14] = ref_cache_l1[30] = -1;

				GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, pLayerMmo->mb_slice_table, curr_residu, Pps -> constrained_intra_pred_flag);

				decode_MB_I(&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
					&V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
					Pps, &curr_residu[iCurrMbAddr],PicWidthInMbs, quantif, vectors -> baseline_vectors);

				ResetMbResidu(&Residu_Luma[((iMb_x + (iMb_y * PicWidthInMbs << 4))<< 4)], 
					&Residu_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
					&Residu_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], PicWidthInMbs << 4);

				curr_residu [iCurrMbAddr] . blk4x4_Res = 0;

			}else if ( Nal -> TCoeffPrediction && IS_I_SVC (curr_residu [iCurrMbAddr] . MbType) && IS_I (base_residu [iCurrMbAddr] . MbType) ){
				//In case of a TLevelCoeff, I base macroblock and SNR layer
				//Initialization of the macroblock neighbourhood
				GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, pLayerMmo->mb_slice_table, curr_residu, Pps -> constrained_intra_pred_flag);

				curr_residu [iCurrMbAddr] . MbType = base_residu [iCurrMbAddr] . MbType;
				curr_residu [iCurrMbAddr] . Intra16x16PredMode = base_residu [iCurrMbAddr] . Intra16x16PredMode;
				curr_residu [iCurrMbAddr] . IntraChromaPredMode = base_residu [iCurrMbAddr] . IntraChromaPredMode;

				//Macroblock decoding process
				Decode_I_BL(&curr_residu [iCurrMbAddr], &base_residu [iCurrMbAddr], Pps, PicWidthInMbs, quantif, vectors -> baseline_vectors, 
					Y + ((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4), U + ((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3), 
					V + ((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3));

			}else {
				// Search the base layer prediction.
				short mbBaseAddr = GetBasePositionB(Sps -> direct_8x8_inference_flag, iMb_x << 4,iMb_y << 4, &x_base, &y_base, 
					&x_mbPartIdx, &y_mbPartIdx, Nal, RefPartitionL0, &ai_tstTab_Block [iCurrMbAddr], &curr_residu[iCurrMbAddr], 
					base_residu, mv_base_l0, ref_base_l0,  mv_base_l1, ref_base_l1, mv_cache_l0, mv_cache_l1 );


				//Bring back motion vectors and reference indeiMb_xes needed.
				fill_caches_motion_vector_B_full_ref(Slice, Nal -> b_stride, Nal -> b8_stride, 
					ref_cache_l0, ref_cache_l1, mv_cache_l0, mv_cache_l1, pLayerMmo->mb_slice_table, curr_residu, 
					&mv_l0[iMb_x * 8 + iMb_y * (Nal -> b_stride << 2)], &mv_l1[iMb_x * 8 + iMb_y * (Nal -> b_stride << 2)], 
					&ref_l0[iMb_x * 4 + iMb_y * (Nal -> b_stride << 1)], &ref_l1[iMb_x * 4 + iMb_y * (Nal -> b_stride << 1)], iCurrMbAddr, iMb_x);

				//In case of an BL macroblock
				if (IS_BL(curr_residu [iCurrMbAddr] . MbType)){ 
					Decode_B_BL(Pps, Sps, Nal, &curr_residu[iCurrMbAddr], base_residu, vectors -> baseline_vectors, quantif, ai_pstRefPicListL0, 
						ai_pstRefPicListL1, iMb_x, iMb_y, x_base, y_base, x_mbPartIdx, y_mbPartIdx, PicWidthInMbs << 4, RefPartitionL0, 
						ref_cache_l0, ref_cache_l1, mv_cache_l0, mv_cache_l1, mv_base_l0, mv_base_l1, ref_base_l0, ref_base_l1, 
						 &Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], 
						&U[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], &V[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
						&Residu_Luma[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)]	, &Residu_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
						&Residu_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)]);

				}else{
					//For B tiMb_ype macroblock
					svc_vectors -> B_vectors [curr_residu [iCurrMbAddr] . Mode](mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, 
						&curr_residu [iCurrMbAddr], &ai_tstTab_Block [iCurrMbAddr], svc_vectors -> B_sub_vectors, 
						&Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
						&V[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], iMb_x << 4 , iMb_y << 4, PicWidthInMbs << 4, 
						PicHeightInMbs << 4, Nal, ai_pstRefPicListL0, ai_pstRefPicListL1, mv_base_l0, mv_base_l1, 
						ref_base_l0, ref_base_l1, vectors -> baseline_vectors, RefPartitionL0);


					// Add CurrResidual to the prediction
					AddResidual(Nal, Pps, &curr_residu[iCurrMbAddr], &base_residu[mbBaseAddr], PicWidthInMbs << 4, quantif, 
						&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
						&V[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], &Residu_Luma[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], 
						&Residu_Cb[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], &Residu_Cr[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)]);
				}
			}

			//Recording of the motion vector
			write_back_motion_full_ref(Nal -> b_stride, Nal -> b8_stride, &mv_l0[iMb_x * 8 + iMb_y * (Nal -> b_stride << 2)], mv_cache_l0, 
				&ref_l0[iMb_x * 4 + iMb_y * (Nal -> b_stride << 1)], ref_cache_l0 );
			write_back_motion_full_ref(Nal -> b_stride, Nal -> b8_stride, &mv_l1[iMb_x * 8 + iMb_y * (Nal -> b_stride << 2)], mv_cache_l1, 
				&ref_l1[iMb_x * 4 + iMb_y * (Nal -> b_stride << 1)], ref_cache_l1 );

			//Updating the macroblock address
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs,PicWiInMbs, pLayerMmo->mb_slice_table, &iMb_x, &iMb_y);
			if ( pLayerMmo->mb_slice_table [iCurrMbAddr] == 255 ) {
				iCurrMbAddr = PicSizeInMbs ;
			}
		}
	}
}

