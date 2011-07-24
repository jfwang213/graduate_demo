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

#ifndef NAL_UNIT_H
#define NAL_UNIT_H


#include "svc_type.h"
#include "decoder_context.h"

void init_nal_struct(NAL *Nal, unsigned char MaxNumLayer);
void IsPicToDecode(NAL *Nal);
void SetBaseNalToDisplay(NAL *Nal, int EndOfSlice,decoder_context* pdecoder_context);
void SetDefRecPic(NAL *Nal, unsigned char *do_dec_ref_pic_marking_base);
void SetPicToDisplay(NAL *Nal);
void CheckPicToDisplay(NAL *Nal, int EndOfSlice,decoder_context* pdecoder_context);
void CheckSkipPicToDisplay(NAL *Nal, int EndOfSlice,decoder_context *pdecoder_context);
void NextNalDqId(ConfigSVC *StreamType, NAL *Nal, int *SetLayer, int DqIdMax);
void PrefixNalUnit(unsigned char *Data, int NalInRbsp, NAL *Nal, MMO *Mmo, SPS *Sps, 
				   unsigned char *DecRefPic, unsigned char *EndOfSlice,decoder_context *pdecoder_context);
void NalUnitSVC(unsigned char *data_in, NAL *Nal);
void subset_sps (unsigned char *data, int NalInRbsp, SPS *sps, NAL *Nal);
void NalUnitHeader (unsigned char *Data, int *position, unsigned char *do_dec_ref_pic_marking_base, NAL *Nal, unsigned char *EndOfSlice);

void NalUnitLayerInfo(NAL* nal);
int GetNalBytesInNalunit ( unsigned char *data, NAL * current_nal, int *NumBytesInRBSP, ConfigSVC *StreamType);
void NalUnitData(unsigned char *Data, int *position, NAL *Nal, unsigned char *EndOfSlice);

#endif
