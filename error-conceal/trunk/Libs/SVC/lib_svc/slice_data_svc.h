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


#ifndef SLICE_DATA_SVC_H
#define SLICE_DATA_SVC_H
#include "svc_type.h"



int slice_data_cavlc_svc(const NAL *Nal, RESIDU *Current_residu, RESIDU *BaseResidu, const unsigned char *ai_pcData,  
						  const int ai_piNalBytesInNalunit, const SPS *Sps, const PPS *Pps, DATA aio_tstBlock [], 
						  SLICE *Slice, int *position, const VLC_TABLES *vlc,  unsigned char *aio_tiSlice_table, 
						  const  unsigned char *MbToSliceGroupMap, unsigned char *EndOfSlice);


int SliceDataICabacSvc(unsigned char *Data,  const int NalBytesInNalunit, int Pos, const NAL *Nal, const SPS *Sps, 
						const PPS *Pps, SLICE *Slice, RESIDU *CurrResidu, RESIDU *BaseResidu, DATA *Block, 
						unsigned char *SliceTable, unsigned char *MbToSliceGroupMap, unsigned char *EndOfSlice);


int SliceDataPCabacSvc(unsigned char *Data,  const int NalBytesInNalunit, int Pos, const NAL *Nal, const SPS *Sps, 
						const PPS *Pps, SLICE *Slice, RESIDU *CurrResidu, RESIDU *BaseResidu, DATA *Block, 
						unsigned char *SliceTable, unsigned char *MbToSliceGroupMap, unsigned char *EndOfSlice, short *MvCabacL0, short *RefCabacL0); 

int SliceDataBCabacSvc(unsigned char *Data,  const int NalBytesInNalunit, int Pos, const NAL *Nal, const SPS *Sps, 
						const PPS *Pps, SLICE *Slice, RESIDU *CurrResidu, RESIDU *BaseResidu, DATA *Block, 
						unsigned char *SliceTable, unsigned char *MbToSliceGroupMap, unsigned char *EndOfSlice, short *MvCabacL0, 
						short *RefCabacL0, short *MvCabacL1, short *RefCabacL1);




static __inline int is_crop_in_window(const SPS *Sps, int iMb_x, int iMb_y ){

	int LeftBorder = iMb_x >= Sps -> scaled_base_left_offset >> 3;
	int TopBorder = iMb_y >= Sps -> scaled_base_top_offset >> 3;
	int RightBorder = iMb_x < ((Sps -> pic_width_in_mbs << 3) - Sps -> scaled_base_right_offset) >> 3;
	int BottomBorder = iMb_y < ((Sps -> pic_height_in_map_units << 3) - Sps -> scaled_base_bottom_offset) >> 3;

	return LeftBorder && TopBorder && RightBorder && BottomBorder;

}
#endif
