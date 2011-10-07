/*****************************************************************************
*
*  Open SVC Decoder developped in IETR image lab
*
*
*
*              M�d�ric BLESTEL <mblestel@insa-rennes.Fr>
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

//AVC files
#include "data.h"
#include "cavlc.h"
#include "init_data.h"
#include "slice_header.h"
#include "SliceGroupMap.h"

//SVC files
#include "svc_type.h"
#include "svc_data.h"
#include "Coordonates.h"
#include "slice_data_svc.h"
#include "fill_caches_svc.h"
#include "ErrorConcealment.h"
#include "decoder_context.h"
#include "nal_unit_header_svc_extension.h"


/**
This functions initializes the RESIDU structure of a macroblock when a slice is skipped.
*/
void SkipSliceMacroInit (RESIDU *CurrResidu, SLICE *Slice){

	init_residu(CurrResidu);
	if (Slice -> slice_type == SLICE_TYPE_P){ 
		init_P_skip_mb(CurrResidu, Slice -> slice_qp_delta, Slice);
		CurrResidu -> MbType = P_BL;
	}else{
		init_B_skip_mb(CurrResidu, Slice -> slice_qp_delta, Slice);
		CurrResidu -> MbType = B_BL;
	}
	CurrResidu -> InCropWindow = CurrResidu -> BaseModeFlag = CurrResidu -> ResidualPredictionFlag = 1;
}




/**
This functions initializes the RESIDU structure of a skipped slice.
*/
void SkipSliceDecoding(NAL *Nal, const SPS *Sps, SLICE *Slice, RESIDU *CurrResidu, DATA *Block, unsigned char *MbToSliceGroupMap){

	short iMb_x;
	short iMb_y;
	short iCurrMbAddr = Slice -> first_mb_in_slice;
	const short PicSizeInMbs = Sps -> PicSizeInMbs ;
	const short PicWidthInMbs = Sps -> pic_width_in_mbs;

	//initialize macroblock position
	GetMbPosition(iCurrMbAddr, PicWidthInMbs, &iMb_x, &iMb_y);

	for (; iCurrMbAddr < PicSizeInMbs ;) {
		SkipSliceMacroInit(&CurrResidu [iCurrMbAddr], Slice);
		//Initialize base macroblock address
		GetBaseMbAddr(Nal, &Block[iCurrMbAddr], iMb_x << 4, iMb_y << 4);

		//Updating the macroblock address
		iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWidthInMbs, MbToSliceGroupMap, &iMb_x, &iMb_y);
	}
}


/**
CAVLC decoding process for SVC layers.
*/
void slice_data_in_scalable_extension_cavlc ( const int size_mb, const unsigned char *Data, const int NalBytesInNalunit, 
		const int *Position, NAL *Nal, const SPS *Sps, PPS *Pps, const VLC_TABLES *vlc, 
		SLICE *Slice,  DATA *TabBlock, RESIDU *currentResidu, unsigned char *EndOfSlice,MMO *mmo)
{
	if( !Nal -> NalToSkip){
		RESIDU *baseResidu = mmo->LayerMMO[Nal->BaseLayerId].residu;
		LAYER_MMO* pLayerMmo = &(mmo->LayerMMO[Nal->LayerId]);
		int pos = *Position;
		const int MbMacro = Sps -> PicSizeInMbs;

		//Parameters initialization Initialisation in case of a new picture
		if ( *EndOfSlice == 0 ) {
			reinit_slice(Slice);
			Slice -> mb_stride = Sps -> pic_width_in_mbs;
			Nal -> b_stride = Sps -> b_stride;
			Nal -> b8_stride = Sps -> b8_stride;
			mapUnitToSlice(Sps, Pps, pLayerMmo->mb_slice_group_map, Slice -> slice_group_change_cycle, Pps->slice_group_id);
			InitUcharTab(MbMacro, pLayerMmo->mb_slice_table);
		}else{
			SliceManagement(Pps, Slice);
		}



		if ( Nal -> SliceSkipFlag){
			SkipSliceDecoding(Nal, Sps, Slice, currentResidu, TabBlock, pLayerMmo->mb_slice_group_map);
			memset(pLayerMmo->mb_slice_table, 0, MbMacro* sizeof(int)); 

		}else {
			Nal -> ErrorDetection = slice_data_cavlc_svc(Nal, currentResidu, baseResidu, 
				Data, NalBytesInNalunit, Sps, Pps, TabBlock, Slice, &pos, vlc, pLayerMmo->mb_slice_table, 
				pLayerMmo->mb_slice_group_map, EndOfSlice );
		}
	}


#ifdef ERROR_CONCEALMENT
	//Error concealment
	SVCErrorConcealment(Nal, Sps, Slice, pLayerMmo->mb_slice_table, currentResidu);
#endif
}






/**
CABAC decoding process for SVC layers.
*/
void SliceCabac(const int size_mb, unsigned char *data, int NalBytesInNalunit, int *position, NAL *Nal, const SPS *Sps, PPS *Pps, 
				 short *mv_cabac_l0, short *mv_cabac_l1, short *ref_cabac_l0, short *ref_cabac_l1, 
				 SLICE *Slice,  DATA *Tab_block, RESIDU *current_residu, unsigned char *EndOfSlice,MMO* mmo)
{



	if( !Nal -> NalToSkip){
		RESIDU* baseResidu = mmo->LayerMMO[Nal->BaseLayerId].residu;
		LAYER_MMO* pLayerMmo = &(mmo->LayerMMO[Nal->LayerId]);
		//Frame with TemporalId >= WantedTemporalid are not decoded
		int pos = *position;
		const int MbMacro = Sps -> PicSizeInMbs;

		//Parameters initialization Initialisation in case of a new picture
		if ( *EndOfSlice == 0 ) {
			reinit_slice(Slice);
			Slice -> mb_stride = Sps -> pic_width_in_mbs;
			Nal -> b_stride = Sps -> b_stride;
			Nal -> b8_stride = Sps -> b8_stride;
			mapUnitToSlice(Sps, Pps, pLayerMmo->mb_slice_group_map, Slice -> slice_group_change_cycle, Pps->slice_group_id);
			InitUcharTab(MbMacro, pLayerMmo->mb_slice_table);
		}else{
			SliceManagement(Pps, Slice);
		}


		//CABAC decoding process
		if((pos & 7) != 0)  {
			pos =((pos >> 3) + 1) << 3;
		}



		if ( Nal -> SliceSkipFlag){
			SkipSliceDecoding(Nal, Sps, Slice, current_residu, Tab_block, pLayerMmo->mb_slice_group_map);
			memset(pLayerMmo->mb_slice_table, 0, MbMacro * sizeof(int)); 
		}else {
			if ( Slice -> slice_type == SLICE_TYPE_B){
				Nal -> ErrorDetection = SliceDataBCabacSvc(data, NalBytesInNalunit, pos, Nal, Sps, Pps, Slice, current_residu, 
					baseResidu, Tab_block, pLayerMmo->mb_slice_table, pLayerMmo->mb_slice_group_map, EndOfSlice, 
					mv_cabac_l0, ref_cabac_l0, mv_cabac_l1, ref_cabac_l1);

			}else if ( Slice -> slice_type == SLICE_TYPE_P){
				Nal -> ErrorDetection = SliceDataPCabacSvc(data, NalBytesInNalunit, pos, Nal, Sps, Pps, Slice, current_residu, 
					baseResidu, Tab_block, pLayerMmo->mb_slice_table, pLayerMmo->mb_slice_group_map, EndOfSlice, 
					mv_cabac_l0, ref_cabac_l0);

			}else {
				Nal -> ErrorDetection = SliceDataICabacSvc(data, NalBytesInNalunit, pos, Nal, Sps, Pps, Slice, current_residu, 
					baseResidu, Tab_block, pLayerMmo->mb_slice_table, pLayerMmo->mb_slice_group_map, EndOfSlice);
			}
		}
	}

	
#ifdef ERROR_CONCEALMENT
	//Error concealment
	SVCErrorConcealment(Nal, Sps, Slice, pLayerMmo->mb_slice_table, current_residu);
#endif
}
