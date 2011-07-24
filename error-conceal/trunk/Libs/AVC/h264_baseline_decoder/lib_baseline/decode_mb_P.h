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



#ifndef MB_P_H
#define MB_P_H

#include "type.h"




void decode_P_Intra4x4_lum ( unsigned char image [], const RESIDU *residu, const short PicWidthInPix, const short W4x4_inter [6][16]);
void decode_P_Intra8x8_lum ( unsigned char image [], const RESIDU *residu, const short PicWidthInPix, const short W8x8_inter [6][64]);

void decode_P_chroma ( unsigned char image_Cb [], unsigned char image_Cr [], const PPS *pps, const RESIDU *residu, const short PicWidthInPix, const W_TABLES *quantif);



#endif
