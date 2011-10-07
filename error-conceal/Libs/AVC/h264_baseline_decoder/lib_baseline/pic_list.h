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

#ifndef PIC_LIST_H
#define PIC_LIST_H


#include "type.h"
#include "svc_type.h"





void fill_default_ref_list(DEP_MMO* dep_mmo,LIST_MMO *refPicLXl0,LIST_MMO *refPicLXl1  ,LIST_MMO *current, int slice_type,char bUserBaseLayer);
void PocComputation(SPS *sps, int IdrPicFlag, int nal_ref_idc, MMO *Mmo, 
					LAYER_MMO *LayerMmo, LIST_MMO *Current_pic,SLICE* current_slice);
void fill_frame_num_gap(MMO *mmo, int baseLayerId, int MaxFrameNum, int num_ref_frames, 
						LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[],SLICE* currentSlice,int dq_id_display);
void pic_order_process(SPS *sps, int IdrPicFlag, int nal_ref_idc, MMO *Mmo, LAYER_MMO *LayerMmo, 
					   LIST_MMO *Current_pic, LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[],
					   SLICE* current_slice,int dq_id_display);
void execute_ref_pic_marking(SPS* sps,MMO *Mmo, int baseLayerId,int highestLayerId);
void direct_ref_list_init(SLICE *aio_pstSlice, LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[], LIST_MMO *Current_pic, LAYER_MMO *LayerMmo);
LIST_MMO* findPocFrame(LIST_MMO* list,int poc,int listNum);

void ref_pic_marking_idr(int baseLayerId,MMO *Mmo,LAYER_MMO* baseLayerMmo,LIST_MMO* baseLayerPic,
						 LIST_MMO *highestLayerPic,char bDealBaseLayer);

#endif


