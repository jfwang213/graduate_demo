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
#include "type.h"
#include "non_IDR_picture.h"
#include "init_data.h"
#include "neighbourhood.h"
#include "decode_mb_I.h"
#include "decode_mb_P.h"
#include "SliceGroupMap.h"
#include "interpolation.h"
#include "interpolation_8x8.h"
#include "write_back.h"




/**
This function allows to build of a P picture. 


@param sps The sps structure.
@param pps The pps structure.
@param aio_pstSlice The slice structure contains all information used to decode the next slice
@param aio_pstSliceGroupMap Specifies in which aio_pstSlice belongs each macroblock of the picture.
@param aio_pstSlice_table Specifies in which aio_pstSlice belongs each macroblock of the picture.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param aio_tstIntra_pre A table where each prediction mode calculated is stocked for each macroblock decoded.
@param picture_residu The structure which contains all the residual data of the macroblocks.
@param ai_struct_pf Function pointer for interpolation
@param ai_pstRefPicListL0 Reference list l0.
@param mv A cache table where the motion vector are stocked for each 4x4 block of each macroblock.
@param ref A cache table where the reference is stocked for each 4x4 block of each macroblock.
@param aio_tucDpb_luma Table of reference frames.
@param aio_tucDpb_Cb Table of reference frames.
@param aio_tucDpb_Cr Table of reference frames.
@param aio_tucImage Table of current frame.
@param aio_tucImage_Cb Table of current frame.
@param aio_tucImage_Cr Table of current frame.
@param quantif W_TABLES which contains all the quantization table.
*/
void Slice_data_P( const SPS *Sps, const PPS *Pps, const SLICE *Slice,  const unsigned char aio_pstSlice_table [ ], 
				  RESIDU *picture_residu, const STRUCT_PF *ai_STRUCT_PF, LIST_MMO *ai_pstRefPicListL0, short aio_tiMv [ ], 
				  short aio_tiRef [ ], unsigned char aio_tucDpb_luma[ ], unsigned char aio_tucDpb_Cb[ ], 
				  unsigned char  aio_tucDpb_Cr[ ], unsigned char aio_tucImage[ ], unsigned char aio_tucImage_Cb[ ], 
				  unsigned char  aio_tucImage_Cr[ ], const W_TABLES *quantif)
{

	__declspec(align(64)) short ref_cache_l0 [48];
	__declspec(align(64)) short mv_cache_l0 [48][2];
	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
	const short PicWiInMbs = Sps -> pic_width_in_mbs;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;
	const short PicHeightInMbs =  Sps -> pic_height_in_map_units + 2; 
	const short PicWidthInPix = PicWidthInMbs << 4 ;
	const short offset_L = (PicWidthInMbs << 8) + 16;
	const short offset_C = (PicWidthInMbs << 6) + 8;  

	short iCurrMbAddr = Slice -> first_mb_in_slice ;

	short iMb_x;
	short iMb_y;
	
	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);


	aio_tucImage += offset_L;
	aio_tucImage_Cb += offset_C;
	aio_tucImage_Cr += offset_C;



	//Loop on all macroblocks
	for ( ; iCurrMbAddr < PicSizeInMbs ;) {
		unsigned char *Y = &aio_tucImage[((iMb_x + ( iMb_y * PicWidthInPix)) << 4)];
		unsigned char *U = &aio_tucImage_Cb[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)];
		unsigned char *V = &aio_tucImage_Cr[((iMb_x + ( iMb_y * PicWidthInPix >> 1)) << 3)];


		//Construction process for one macroblock
		// I_PCM || INTRA_4x4 || INTRA_16x16
		if ( IS_I(picture_residu [iCurrMbAddr] . MbType) ) 	{
			//Initalization of the neighbourhood
			GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, aio_pstSlice_table, picture_residu, Pps -> constrained_intra_pred_flag);
			decode_MB_I(Y, U, V, Pps, &picture_residu[iCurrMbAddr], PicWidthInMbs, quantif, ai_STRUCT_PF);
		} 
		else {

			if( picture_residu [iCurrMbAddr] . Mode == 0){
				WriteBackMotionCacheCurrPosition( Slice -> b_stride, Slice -> b8_stride, 
					&aio_tiMv[iMb_x * 8 + iMb_y * (Slice -> b_stride << 2)],  mv_cache_l0, 
					&aio_tiRef[iMb_x * 2 + iMb_y * (Slice -> b8_stride << 1)], ref_cache_l0);

				//Recovery of  the prediction
				sample_interpolation(Y, U, V, mv_cache_l0, ref_cache_l0, iMb_x << 4 , iMb_y << 4, PicWidthInMbs << 4, PicHeightInMbs << 4, 
					ai_pstRefPicListL0, ai_STRUCT_PF -> Ptr_interpol_4x4);
			}else{
				write_back_motion_cache_interpolation_8x8( Slice -> b_stride,  Slice -> b8_stride, 
					&aio_tiMv[iMb_x * 8 + iMb_y * ( Slice -> b_stride << 2)],  mv_cache_l0, 
					&aio_tiRef[iMb_x * 2 + iMb_y * (Slice -> b8_stride << 1)], ref_cache_l0);

				SampleInterpolation8x8(Y, U, V, mv_cache_l0, ref_cache_l0, iMb_x << 4 , iMb_y << 4, PicWidthInMbs << 4, 
					PicHeightInMbs << 4, ai_pstRefPicListL0, ai_STRUCT_PF -> Ptr_interpol_8x8);
			}


			//Add of the residual
			if ( !picture_residu [iCurrMbAddr] . Transform8x8)
				decode_P_Intra4x4_lum(Y, &picture_residu [iCurrMbAddr], PicWidthInPix, quantif -> W4x4_inter);
			else
				decode_P_Intra8x8_lum(Y, &picture_residu [iCurrMbAddr], PicWidthInPix, quantif -> W8x8_inter);


			//Decode the chrominance
			decode_P_chroma(U, V, Pps , &picture_residu[iCurrMbAddr], PicWidthInPix >> 1, quantif);
		}


		//Updating the macroblock address
		iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWiInMbs, aio_pstSlice_table, &iMb_x, &iMb_y);
		if ( aio_pstSlice_table [iCurrMbAddr] == 255 ) {
			iCurrMbAddr = PicSizeInMbs ;
		}
	}
}



/**
This function allows to build of a P picture. 


@param Sps The SPS structure.
@param Pps The PPS structure.
@param aio_pstSlice The slice structure contains all information used to decode the next slice
@param aio_pstSliceGroupMap Specifies in which aio_pstSlice belongs each macroblock of the picture.
@param aio_pstSlice_table Specifies in which aio_pstSlice belongs each macroblock of the picture.
@param aio_tstBlock Contains all parameters of each macroblock of the current picture.
@param aio_tstIntra_pre A table where each prediction mode calculated is stocked for each macroblock decoded.
@param picture_residu The structure which contains all the residual data of the macroblocks.
@param All_PF_struct Function pointer for interpolation
@param ai_pstRefPicListL0 Refernce list l0.
@param Current_pic LIST_MMO structure which contains the data of the current picture.
@param quantif W_TABLES which contains all the quantization table.
@param mv A cache table where the motion vector are stocked for each 4x4 block of each macroblock.
@param ref A cache table where the reference is stocked for each 4x4 block of each macroblock.
@param aio_tucDpb_luma Table of reference frames.
@param aio_tucDpb_Cb Table of reference frames.
@param aio_tucDpb_Cr Table of reference frames.
*/
void non_IDR_picture ( 	const SPS *Sps, const PPS *Pps,const SLICE *Slice, const unsigned char aio_pstSlice_table [ ], 
					  RESIDU *picture_residu, const STRUCT_PF *All_PF_struct, LIST_MMO *ai_pstRefPicListL0,
					  LIST_MMO *Current_pic, const W_TABLES *quantif, short aio_tiMv [ ], short aio_tiRef [ ], 
					  unsigned char aio_tucDpb_luma[ ], unsigned char aio_tucDpb_Cb[ ], unsigned char  aio_tucDpb_Cr[ ])
{

	char* luma_memory = Current_pic->picture_memory_y;
	char* cb_memory = Current_pic->picture_memory_u;
	char* cr_memory = Current_pic->picture_memory_v;
	if (Sps -> PicSizeInMbs){ 
		Slice_data_P( Sps, Pps, Slice, aio_pstSlice_table, picture_residu, All_PF_struct, ai_pstRefPicListL0, aio_tiMv, aio_tiRef, 
		aio_tucDpb_luma, aio_tucDpb_Cb, aio_tucDpb_Cr, luma_memory, cb_memory, cr_memory, quantif);
	}
}
