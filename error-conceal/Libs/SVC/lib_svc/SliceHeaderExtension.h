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

#ifndef SLICE_HEADER_EXT_H
#define SLICE_HEADER_EXT_H


//AVC Files
#include "type.h"











int SliceHeaderCut(unsigned char *data, SPS *sps, PPS *pps, NAL *Nal, int *position);
int SpatialScalabilityType(SPS *sps, NAL *Nal );
void NalParamInit(NAL *Nal, PPS *pps, SPS *sps, SPS *pt_sps_id);
void SetSpatialUsed(NAL *Nal);
void SvcReadDefaultParameters(unsigned char *data, int *position, NAL *Nal, SLICE *Slice, SPS *pt_sps_id);
void SvcReadLayerPrediction(unsigned char *data, int *position, NAL *Nal, SPS *pt_sps_id, PPS *pt_pps_id, SLICE *Slice, 
							LIST_MMO *Current_pic, MMO *Mmo, int EndOfSlice);
void SvcReadReferenceListProcessing(unsigned char *data, int *position, NAL *Nal, SLICE *Slice, SPS *pt_sps_id, 
								PPS *pt_pps_id, LAYER_MMO *LayerMmo, LIST_MMO *Current_pic, MMO *Mmo, 
								LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1);
#endif
