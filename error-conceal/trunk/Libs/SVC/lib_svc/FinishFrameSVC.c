
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
//#include "type.h"
#include "symbol.h"
#include "pic_list.h"
#include "display_management.h"


//SVC Files
#include "svc_type.h"
#include "set_layer.h"
#include "SVCDisplay.h"
#include "pic_list_svc.h"
#include "Loop_filter_ext.h"

#include "layer_utils.h"
#include "log.h"



/**
Used when error has been detected into a slice, to execute the MMO operation and the display management
*/
void FinishFrameSVCError(int Execute, NAL *Nal, SPS *StructSPS, PPS *Pps, LIST_MMO *Current_pic, int *Crop, 
						 int *ImgToDisplay, DISPLAY_M * display_memory, MMO *Mmo, int *xsize, int *ysize)
{
	SPS *sps_id;
	int pic_parameter_set_id;

	pic_parameter_set_id = Nal -> pic_parameter_id [Nal -> LayerId];
	sps_id = &StructSPS[Pps[pic_parameter_set_id] . seq_parameter_set_id + (Nal -> LayerId ? 16: 0)];


	//Sps must be defined
	if(Execute){
		//Short and long term management
		int baseLayerId = GetLayerNumber(Nal,Nal->DqId>>4<<4);
		int highDQId = Nal->DqId < Nal->DqIdToDisplay ? Nal->DqId:Nal->DqIdToDisplay;
		int highLayerId = GetLayerNumber(Nal,highDQId);
		execute_ref_pic_marking(StructSPS,Mmo,baseLayerId,highLayerId);
	}

	//Display managment.
	DisplayOrderSvc(NULL, display_memory, xsize, ysize, Crop, ImgToDisplay, Mmo);
}





void FinishFrameSVC( NAL *Nal, SPS *Sps, PPS *Pps, LIST_MMO *Current_pic, SLICE *Slice, int EndOfSlice, 
					DATA *TabBlbock, RESIDU *Residu, MMO *Mmo)
{


	if (!EndOfSlice && Mmo -> MemoryAllocation){
		LAYER_MMO *LayerMmo = &(Mmo->LayerMMO[Nal->LayerId]);
		DEP_MMO* dep_mmo = LayerMmo->dep_mmo;
		if ( Nal -> PicToDisplay && Nal->bFinishLayerRepresent){
			const int PicWidthInPix = (Sps -> pic_width_in_mbs + 2) << 4;
			const int PicHeightInPix = (Sps -> pic_height_in_map_units ) << 4;
			short *mvL0 = Current_pic->mv_memory_l0;
			short *mvL1 = Current_pic->mv_memory_l1;
			short *refL0 = Current_pic->ref_memory_l0;
			short *refL1 = Current_pic->ref_memory_l1;
			unsigned char *Y = Current_pic->picture_memory_y;
			unsigned char *U = Current_pic->picture_memory_u;
			unsigned char *V = Current_pic->picture_memory_v;
			//Deblocking filter
			if (!Nal -> DqId){
				Loop_filter_avc(Sps, Pps, Slice, Current_pic, LayerMmo->mb_slice_table, TabBlbock, mvL0, mvL1, refL0, refL1, Residu, Y, U, V);
			}else {
				Loop_filter_svc(Nal, Sps, Pps, Slice, Current_pic, LayerMmo->mb_slice_table, mvL0, mvL1, refL0, refL1, Residu, Y, U, V);
			}
			//Padding for motion interpolation
			ImageSetEdges(Y, U, V, PicWidthInPix, PicHeightInPix);
			logStr("add to poc buffer, poc:%d, pic number:%d\tmemory:%x\n",Current_pic->poc,
					Mmo->nb_img_display - Mmo -> num_free_poc_address,Current_pic->picture_memory_y);
			StorePicture(&(Mmo->poc_params[Mmo->nb_img_display - Mmo -> num_free_poc_address]),
					Current_pic,Sps,Nal->DqIdToDisplay);
			Mmo->num_free_poc_address--;
		}
	}
}
