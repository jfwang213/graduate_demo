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

//AVC Files
#include "type.h"
#include "data.h"
#include "init_data.h"
#include "main_data.h"
#include "decode_mb_I.h"
#include "decode_mb_P.h"
#include "fill_caches.h"
#include "SliceGroupMap.h"
#include "neighbourhood.h"
#include "fill_caches_svc.h"
#include "residual_chroma.h"
#include "fill_caches_cabac.h"
#include "interpolation_main.h"
#include "interpolation_main_8x8.h"


//SVC Files
#include "blk4x4.h"
#include "residual_4x4.h"
#include "residual_8x8.h"
#include "WriteBackSVC.h"
#include "Slice_data_avc.h"
#include "ResidualProcessing.h"




void Decode_B_avc ( SPS *ai_stSps, PPS *ai_stPps, SLICE *ai_stSlice, unsigned char ai_tSlice_table [], 
				   RESIDU *picture_residu, MAIN_STRUCT_PF *main_vectors, LIST_MMO *ai_pRefPicListL0, 
				   LIST_MMO *ai_pRefPicListL1, LIST_MMO *Current_pic, const W_TABLES *quantif, 
				   const NAL *Nal)
{

	if(!Nal -> NalToSkip){
		__declspec(align(64)) short ref_cache_l0 [48];
		__declspec(align(64)) short ref_cache_l1 [48];
		__declspec(align(64)) short mv_cache_l0 [48][2];
		__declspec(align(64)) short mv_cache_l1 [48][2];

		__declspec(align(64)) unsigned char image_l0[256];
		__declspec(align(64)) unsigned char image_l1[256];
		__declspec(align(64)) unsigned char image_Cb_l0[64];
		__declspec(align(64)) unsigned char image_Cb_l1[64];
		__declspec(align(64)) unsigned char image_Cr_l0[64];
		__declspec(align(64)) unsigned char image_Cr_l1[64];


		const short PicSizeInMbs = (short)ai_stSps -> PicSizeInMbs ;
		const short PicWiInMbs = (short)ai_stSps -> pic_width_in_mbs;
		const short PicWidthInMbs = (short)ai_stSps -> pic_width_in_mbs + 2;
		const short PicWidthInPix = (short)PicWidthInMbs << 4 ;
		const short PicHeightInPix =  (short)(ai_stSps -> pic_height_in_map_units + 2) << 4;


		short* residu_luma = Current_pic->residual_memory_y;
		short* residu_cb = Current_pic->residual_memory_u;
		short* residu_cr = Current_pic->residual_memory_v;

		short *aio_tMv_l0 = Current_pic->mv_memory_l0;
		short *aio_tMv_l1 = Current_pic->mv_memory_l1;
		short *aio_tref_l0 = Current_pic->ref_memory_l0;
		short *aio_tref_l1 = Current_pic->ref_memory_l1;

		const short offset_L = (PicWidthInMbs << 8) + 16;
		const short offset_C = (PicWidthInMbs << 6) + 8;  
		
		unsigned char *Y = offset_L + Current_pic->picture_memory_y;
		unsigned char *U = offset_C + Current_pic->picture_memory_u;
		unsigned char *V = offset_C + Current_pic->picture_memory_v;
	 
		short iCurrMbAddr = ai_stSlice -> first_mb_in_slice ;

		short iMb_x;
		short iMb_y;
		
		//initialize macroblock position
		GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);

		

		//Loop on all macroblocks
		for ( ; iCurrMbAddr < PicSizeInMbs ;) {

			if (  IS_I(picture_residu [iCurrMbAddr] . MbType)) {
				//Initalization of the neighbourhood
				GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, ai_tSlice_table, picture_residu, ai_stPps -> constrained_intra_pred_flag);

				decode_MB_I(&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4) ], &U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
					&V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], ai_stPps, &picture_residu[iCurrMbAddr], 
					PicWidthInMbs, quantif, main_vectors -> baseline_vectors);

				ResetMbResidu(&residu_luma[((iMb_x + (iMb_y * PicWidthInMbs << 4))<< 4)], 
					&residu_cb[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
					&residu_cr[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], PicWidthInMbs << 4);
			} else {

				if (Nal -> PicToDisplay){
					if( picture_residu [iCurrMbAddr] . Mode == 0){
						write_back_full_ref( ai_stSlice -> b_stride, ai_stSlice -> b8_stride, 
							&aio_tMv_l0[iMb_x * 8 + iMb_y * (ai_stSlice -> b_stride << 2)],  mv_cache_l0, 
							&aio_tref_l0[iMb_x * 4 + iMb_y * (ai_stSlice -> b_stride << 1)], ref_cache_l0);

						write_back_full_ref( ai_stSlice -> b_stride, ai_stSlice -> b8_stride, 
							&aio_tMv_l1[iMb_x * 8 + iMb_y * (ai_stSlice -> b_stride << 2)],  mv_cache_l1, 
							&aio_tref_l1[iMb_x * 4 + iMb_y * (ai_stSlice -> b_stride << 1)], ref_cache_l1);

						//Recovery of  the prediction
						sample_interpolation_main(image_l0, image_l1, image_Cb_l0, image_Cb_l1, 
							image_Cr_l0, image_Cr_l1, mv_cache_l0, mv_cache_l1, ref_cache_l0, ref_cache_l1, iMb_x << 4, iMb_y << 4, 
							PicWidthInPix, PicHeightInPix , ai_pRefPicListL0, ai_pRefPicListL1, main_vectors -> baseline_vectors -> Ptr_interpol_4x4);

						set_image_4x4(&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)],
							&V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], image_l0, image_l1, image_Cb_l0, image_Cb_l1, 
							image_Cr_l0, image_Cr_l1, ref_cache_l0, ref_cache_l1, PicWidthInPix);
					}else{
						write_back_full_ref_8x8(ai_stSlice -> b_stride, ai_stSlice -> b8_stride, 
							&aio_tMv_l0[iMb_x * 8 + iMb_y * (ai_stSlice -> b_stride << 2)],  mv_cache_l0, 
							&aio_tref_l0[iMb_x * 4 + iMb_y * (ai_stSlice -> b_stride << 1)], ref_cache_l0);

						write_back_full_ref_8x8(ai_stSlice -> b_stride, ai_stSlice -> b8_stride, 
							&aio_tMv_l1[iMb_x * 8 + iMb_y * (ai_stSlice -> b_stride << 2)],  mv_cache_l1, 
							&aio_tref_l1[iMb_x * 4 + iMb_y * (ai_stSlice -> b_stride << 1)], ref_cache_l1);


						SampleInterpolationMain8x8(image_l0, image_Cb_l0, image_Cr_l0, mv_cache_l0, ref_cache_l0, 
							iMb_x << 4 , iMb_y << 4, PicWidthInMbs << 4, PicHeightInPix, ai_pRefPicListL0, main_vectors -> baseline_vectors -> Ptr_interpol_8x8);
						SampleInterpolationMain8x8(image_l1, image_Cb_l1, image_Cr_l1, mv_cache_l1, ref_cache_l1, 
							iMb_x << 4 , iMb_y << 4, PicWidthInMbs << 4, PicHeightInPix, ai_pRefPicListL1, main_vectors -> baseline_vectors -> Ptr_interpol_8x8);

						set_image_8x8(&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4) ], &U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
							&V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], image_l0, image_l1, image_Cb_l0, image_Cb_l1, 
							image_Cr_l0, image_Cr_l1, ref_cache_l0, ref_cache_l1, PicWidthInPix);
					}



					//Add of the residual
					if ( !picture_residu [iCurrMbAddr] . Transform8x8){
						//Recovery of  the prediction
						decode_P_Intra4x4_lum(&Y[((iMb_x + (iMb_y * (PicWidthInMbs << 4))) << 4)], 
							&picture_residu [iCurrMbAddr], PicWidthInPix, quantif -> W4x4_inter);
					}else{
						//Recovery of  the prediction
						decode_P_Intra8x8_lum(&Y[((iMb_x + (iMb_y * (PicWidthInMbs << 4)))<< 4)], 
							&picture_residu [iCurrMbAddr], PicWidthInPix,  quantif -> W8x8_inter);
					}

					//Decode the chrominance
					decode_P_chroma(&U[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], &V[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
						ai_stPps, &picture_residu [iCurrMbAddr],  PicWidthInPix >> 1, quantif);
				}else if ( Nal -> PicToDecode){
					if ( !picture_residu [iCurrMbAddr] . Transform8x8){
						Intra4x4_lum(&residu_luma[((iMb_x + (iMb_y * PicWidthInMbs << 4))<< 4)], 
							&picture_residu [iCurrMbAddr], PicWidthInPix, quantif -> W4x4_inter);
					}else {
						Intra8x8_lum(&residu_luma[((iMb_x + (iMb_y * PicWidthInMbs << 4))<< 4)], 
							&picture_residu [iCurrMbAddr], PicWidthInPix, quantif -> W8x8_inter);
					}

					//Decode the chrominance
					decode_chroma(&residu_cb[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], &residu_cr[((iMb_x + (iMb_y * PicWidthInMbs << 3))<< 3)], 
						&picture_residu [iCurrMbAddr], ai_stPps, PicWidthInPix >> 1, quantif);

					update_blk4x4Res(&residu_luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], &picture_residu [iCurrMbAddr], PicWidthInMbs << 4);
				}else{
					ResetMbResidu(&residu_luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], 
						&residu_cb[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
						&residu_cr[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], PicWidthInMbs << 4);
				}
			}

			//Updating the macroblock address
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWiInMbs, ai_tSlice_table, &iMb_x, &iMb_y);
			if ( ai_tSlice_table [iCurrMbAddr] == 255 ) 	{
				iCurrMbAddr = PicSizeInMbs ;
			}
		}
	}
}
