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


void PrefixNalUnit(unsigned char *Data, int NalInRbsp, NAL *Nal, MMO *Mmo, SPS *Sps, 
				   unsigned char *DecRefPic, unsigned char *EndOfSlice,decoder_context *pdecoder_context);
void NalUnitHeader (unsigned char *Data, int *position, unsigned char *do_dec_ref_pic_marking_base, NAL *Nal, unsigned char *EndOfSlice);


void NalUnitData(unsigned char *Data, int *position, NAL *Nal, unsigned char *EndOfSlice);
void NalUnitSVC(unsigned char *data_in,  NAL *Nal);

#endif
