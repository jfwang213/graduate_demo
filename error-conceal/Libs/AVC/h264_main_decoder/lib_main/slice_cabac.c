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
#include "data.h"
#include "main_data.h"
#include "init_data.h"
#include "cavlc_main.h"
#include "slice_header.h"
#include "slice_data_cabac.h"





/**
Cabac decoding process
*/
void slice_layer_cabac ( unsigned char *data, int NalBytesInNalunit, int *aio_piPosition, SPS *sps, PPS *pps, 
						unsigned char *Slice_group_id, LIST_MMO *Current_pic, LIST_MMO *ai_pRefPicListL1, 
						short mv_cabac_l0[], short mv_cabac_l1[], short ref_cabac_l0[], short ref_cabac_l1[], 
						SLICE *Slice, unsigned char MbToSliceGroupMap [], unsigned char Slice_table [], 
						DATA Tab_block[], RESIDU *picture_residu, unsigned char *end_of_Slice,
						short *mvl0_io, short *mvl1_io, short *refl0_io, short *refl1_io) 
{



	if(sps -> PicSizeInMbs){
		int pos = *aio_piPosition;

		//Parameters initialization Initialisation in case of a new picture
		if ( *end_of_Slice == 0 ) {
			reinit_slice(Slice);
			Slice -> mb_stride = sps -> pic_width_in_mbs;
			Slice -> b_stride = sps -> b_stride;
			Slice -> b8_stride = sps -> b8_stride;
			mapUnitToSlice(sps, pps, MbToSliceGroupMap, Slice -> slice_group_change_cycle, Slice_group_id);
			InitUcharTab(sps -> PicSizeInMbs, Slice_table);
		}else{
			SliceManagement(pps, Slice);
		}



		//CABAC decoding process
		if((pos & 7) != 0) {
			pos =((pos >> 3) + 1) << 3;
		}

		if ( Slice -> slice_type == SLICE_TYPE_B){
			slice_data_B_cabac (picture_residu, data, NalBytesInNalunit, sps, pps, 
				Tab_block, Slice, &pos, Slice_table, MbToSliceGroupMap, mv_cabac_l0,
				mv_cabac_l1, ref_cabac_l0, ref_cabac_l1, end_of_Slice, ai_pRefPicListL1, 
				Current_pic->mv_memory_l0, Current_pic->mv_memory_l1, 
				Current_pic->ref_memory_l0, Current_pic->ref_memory_l1,  
				ai_pRefPicListL1[0] . long_term, ai_pRefPicListL1[0] . slice_type, 
				ai_pRefPicListL1[0].mv_memory_l0, ai_pRefPicListL1[0].mv_memory_l1, 
				ai_pRefPicListL1[0].ref_memory_l0, ai_pRefPicListL1[0].ref_memory_l1);

		}else if ( Slice -> slice_type == SLICE_TYPE_P){
			slice_data_P_cabac(picture_residu, data, NalBytesInNalunit, sps, pps, Tab_block, 
				Slice, &pos, Slice_table, MbToSliceGroupMap, mv_cabac_l0, ref_cabac_l0, end_of_Slice, 
				Current_pic->mv_memory_l0,Current_pic->ref_memory_l0);
		}else {
			slice_data_I_cabac (picture_residu, data, NalBytesInNalunit, sps, pps, 
				Tab_block, Slice, &pos, Slice_table, MbToSliceGroupMap, end_of_Slice );
		}
	}
}







/**
Cavlc decoding process
*/
void slice_layer_cavlc ( unsigned char *ai_pcData, const int ai_piNalBytesInNalunit, const int *aio_piPosition, 
						const SPS *Sps, PPS *Pps, VLC_TABLES *Vlc, unsigned char *ai_aio_pstSlice_group_id, LIST_MMO *Current_pic, 
						LIST_MMO *ai_pRefPicListL1, SLICE *Slice, unsigned char *aio_tiMbToSliceGroupMap, unsigned char  aio_pstSlice_table[], 
						DATA aio_tstBlock[], RESIDU *picture_residu, unsigned char *End_of_Slice, 
						short *mvl0_io, short *mvl1_io, short *refl0_io, short *refl1_io) 
{


	if(Sps -> PicSizeInMbs){
		int pos = *aio_piPosition;


		//Parameters initialization Initialisation in case of a new picture
		if ( *End_of_Slice == 0 ) {
			reinit_slice(Slice);
			Slice -> b_stride = Sps -> b_stride;
			Slice -> b8_stride = Sps -> b8_stride;
			Slice -> mb_stride = Sps -> pic_width_in_mbs;
			mapUnitToSlice(Sps, Pps, aio_tiMbToSliceGroupMap, Slice -> slice_group_change_cycle, ai_aio_pstSlice_group_id);
			InitUcharTab(Sps -> PicSizeInMbs, aio_pstSlice_table);
		}else{
			SliceManagement(Pps, Slice);
		}



		if(Slice -> slice_type == SLICE_TYPE_B){
			slice_data_B_cavlc (picture_residu, ai_pcData, ai_piNalBytesInNalunit, Sps,
				Pps, aio_tstBlock, Slice, &pos, Vlc, aio_pstSlice_table, 
				aio_tiMbToSliceGroupMap, End_of_Slice, ai_pRefPicListL1, 
				Current_pic->mv_memory_l0, Current_pic->mv_memory_l1, 
				Current_pic->ref_memory_l0, Current_pic->ref_memory_l1, 
				ai_pRefPicListL1[0] .mv_memory_l0, ai_pRefPicListL1[0] .mv_memory_l1, 
				ai_pRefPicListL1[0] .ref_memory_l0, ai_pRefPicListL1[0] .ref_memory_l1);

		}else if(Slice -> slice_type == SLICE_TYPE_P){
			slice_data_P_cavlc (picture_residu, ai_pcData, ai_piNalBytesInNalunit, Sps, 
				Pps, aio_tstBlock, Slice, &pos, Vlc, aio_pstSlice_table, aio_tiMbToSliceGroupMap, 
				End_of_Slice, Current_pic->mv_memory_l0,Current_pic->ref_memory_l0);

		}else if (Slice -> slice_type == SLICE_TYPE_I){
			slice_data_I_cavlc (picture_residu, ai_pcData, ai_piNalBytesInNalunit, Sps, Pps, aio_tstBlock, 
				Slice, &pos, Vlc, aio_pstSlice_table, aio_tiMbToSliceGroupMap, End_of_Slice);
		}
	}
}
