
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
#include "pic_list.h"
#include "AVCdisplay.h"
#include "Loop_Filter_B.h"
#include "display_management.h"





void FinishFrameMainDecoder(SPS *Sps, PPS *Pps, LIST_MMO *Current_pic, SLICE *Slice, int EndOfSlice, unsigned char *SliceTab, 
							DATA *TabBlbock, RESIDU *Residu , short *MvL0, short *MvL1, short *RefL0, short *RefL1, 
							int *Crop, int *ImgToDisplay, DISPLAY_M* display_memory, MMO *Mmo,
							unsigned char *RefY, unsigned char *RefU, unsigned char *RefV, int *xsize, int *ysize)
{


	if (!EndOfSlice){
		const int PicWidthInPix = (Sps -> pic_width_in_mbs + 2) << 4   ;
		const int PicHeightInPix = (Sps -> pic_height_in_map_units) << 4 ;
		short *mvL0 = Current_pic->mv_memory_l0;
		short *mvL1 = Current_pic->mv_memory_l1;
		short *refL0 =  Current_pic->ref_memory_l0;
		short *refL1 = Current_pic->ref_memory_l1;
		unsigned char *Y = Current_pic->picture_memory_y;
		unsigned char *U = Current_pic->picture_memory_u;
		unsigned char *V = Current_pic->picture_memory_v;
		
		//Short and long term management
		execute_ref_pic_marking(Sps,Mmo,0,0);

		if( Slice -> disable_deblocking_filter_idc != 1){
			//Deblocking filter if necessary
			filter_B(Sps, Pps, Slice, Current_pic, SliceTab, TabBlbock, mvL0, mvL1, refL0, refL1, Residu, Y, U, V);
		}

		//Padding for motion interpolation
		ImageSetEdges(Y, U, V, PicWidthInPix, PicHeightInPix);

		//Display managment.
		display_order(Sps, Current_pic, display_memory, xsize, ysize, Crop, ImgToDisplay, Mmo);
	}
}
