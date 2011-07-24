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

#include "decode_mb_I.h"
#include "decode_mb_P.h"
#include "fill_caches.h"
#include "SliceGroupMap.h"
#include "neighbourhood.h"
#include "interpolation.h"
#include "residual_chroma.h"
#include "fill_caches_svc.h"
#include "interpolation_8x8.h"

//SVC Files
#include "blk4x4.h"
#include "residual_4x4.h"
#include "residual_8x8.h"
#include "WriteBackSVC.h"
#include "Slice_data_avc.h"
#include "ResidualProcessing.h"


/**
This function rebuilds the frame according to datas decoded from the VLD for a P AVC slice.
*/
void Decode_P_avc( const SPS *Sps, const PPS *Pps, const SLICE *ai_pstSlice, const unsigned char ai_tiSlice_table [ ], 
				  RESIDU *picture_residu, const STRUCT_PF * ai_struct_pf, LIST_MMO *ai_pstRefPicListL0, 
				  LIST_MMO * Current_pic,  const W_TABLES *quantif, const NAL *Nal)
{

	if (!Nal -> NalToSkip){
		__declspec(align(64)) short ref_cache_l0 [48];
		__declspec(align(64)) short mv_cache_l0 [48][2];
		const short PicSizeInMbs = Sps -> PicSizeInMbs ;
		const short PicWiInMbs =  Sps -> pic_width_in_mbs;
		const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;
		const short PicHeightInMbs = Sps -> pic_height_in_map_units + 2; 
		const short PicWidthInPix = (PicWidthInMbs << 4);

		const short offset_L = (PicWidthInMbs << 8) + 16;
		const short offset_C = (PicWidthInMbs << 6) + 8; 

		unsigned char *Y = Current_pic->picture_memory_y + offset_L;
		unsigned char *U = Current_pic->picture_memory_u + offset_C;
		unsigned char *V = Current_pic->picture_memory_v + offset_C;

		short* residu_luma = Current_pic->residual_memory_y;
		short* residu_cb = Current_pic->residual_memory_u;
		short* residu_cr = Current_pic->residual_memory_v;

		short* mv = Current_pic->mv_memory_l0;
		short* ref = Current_pic->ref_memory_l0;

		short iCurrMbAddr = ai_pstSlice -> first_mb_in_slice ;

		short iMb_x;
		short iMb_y;
		
		//initialize macroblock position
		GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);

		//Loop on all macroblocks
		for ( ; iCurrMbAddr < PicSizeInMbs ;) {
			if (iCurrMbAddr == 38)
			{
				iCurrMbAddr = 38;
			}
							
			//Construction process for one macroblock
			// I_PCM || INTRA_4x4 || INTRA_16x16
			if ( IS_I(picture_residu [iCurrMbAddr] . MbType) ) {
				//Initalization of the neighbourhood
				GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, ai_tiSlice_table, picture_residu, Pps -> constrained_intra_pred_flag);

				decode_MB_I(&Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], 
					&V[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], Pps, &picture_residu[iCurrMbAddr], PicWidthInMbs, quantif, ai_struct_pf);

				ResetMbResidu(&residu_luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], 
					&residu_cb[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
					&residu_cr[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], PicWidthInMbs << 4);
			} 
			else 
			{
				//Calculate the motion vector
				if (Nal -> PicToDisplay){
					if( picture_residu [iCurrMbAddr] . Mode == 0){
						write_back_full_ref( ai_pstSlice -> b_stride, ai_pstSlice -> b8_stride, 
							&mv[iMb_x * 8 + iMb_y * (ai_pstSlice -> b_stride << 2)],  mv_cache_l0, 
							&ref[iMb_x * 4 + iMb_y * (ai_pstSlice -> b_stride << 1)], ref_cache_l0);

						//Recovery of  the prediction
						sample_interpolation(&Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], 
							&V[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], 
							mv_cache_l0, ref_cache_l0, iMb_x << 4 , iMb_y << 4, 
							PicWidthInMbs << 4, PicHeightInMbs << 4, ai_pstRefPicListL0, ai_struct_pf -> Ptr_interpol_4x4);
					}else{
						write_back_full_ref_8x8(ai_pstSlice -> b_stride, ai_pstSlice -> b8_stride, 
							&mv[iMb_x * 8 + iMb_y * (ai_pstSlice -> b_stride << 2)],  mv_cache_l0, 
							&ref[iMb_x * 4 + iMb_y * (ai_pstSlice -> b_stride << 1)], ref_cache_l0);


						SampleInterpolation8x8(&Y[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], 
							&V[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)], mv_cache_l0, ref_cache_l0, iMb_x << 4 , iMb_y << 4, 
							PicWidthInMbs << 4, PicHeightInMbs << 4, ai_pstRefPicListL0, ai_struct_pf -> Ptr_interpol_8x8);
					}

				
					//Add of the residual
					if ( !picture_residu [iCurrMbAddr] . Transform8x8){
						decode_P_Intra4x4_lum(&Y[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], 
							&picture_residu [iCurrMbAddr], PicWidthInPix, quantif -> W4x4_inter);
					} else{
						decode_P_Intra8x8_lum(&Y[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], 
							&picture_residu [iCurrMbAddr], PicWidthInPix, quantif -> W8x8_inter);
					}

					//Decode the chrominance
					decode_P_chroma(&U[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], &V[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
						Pps, &picture_residu[iCurrMbAddr], PicWidthInPix >> 1, quantif);
				}else if (Nal -> PicToDecode){
					//Cbp test
					if ( !picture_residu [iCurrMbAddr] . Transform8x8){
						Intra4x4_lum(&residu_luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], 
							&picture_residu [iCurrMbAddr], PicWidthInPix, quantif -> W4x4_inter);
					} else {
						Intra8x8_lum(&residu_luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], 
							&picture_residu [iCurrMbAddr], PicWidthInPix, quantif -> W8x8_inter);
					}

					//Decode the chrominance
					decode_chroma(&residu_cb[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
						&residu_cr[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
						&picture_residu [iCurrMbAddr], Pps, PicWidthInPix >> 1, quantif);

				   update_blk4x4Res(&residu_luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], &picture_residu [iCurrMbAddr], PicWidthInMbs << 4);
				}else{
					ResetMbResidu(&residu_luma[((iMb_x + (iMb_y * PicWidthInMbs << 4)) << 4)], 
						&residu_cb[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], 
						&residu_cr[((iMb_x + (iMb_y * PicWidthInMbs << 3)) << 3)], PicWidthInMbs << 4);
				}
			}

			
			//Updating the macroblock address
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWiInMbs, ai_tiSlice_table, &iMb_x, &iMb_y);
			if ( ai_tiSlice_table [iCurrMbAddr] == 255 ) {
				iCurrMbAddr = PicSizeInMbs ;
			}
		}
	}
}

