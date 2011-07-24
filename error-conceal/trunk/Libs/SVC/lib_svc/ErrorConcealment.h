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
#ifndef _ERROR_CONCEALMENT_H_
#define _ERROR_CONCEALMENT_H_

void AVCErrorConcealment(NAL *Nal, const SPS *Sps, SLICE *Slice, unsigned char *SliceTable, RESIDU *Residu,
					  short *MvL0, short *MvL1, short *RefL0, short *RefL1);
void SVCErrorConcealment(NAL *Nal, const SPS *Sps, SLICE *Slice, unsigned char *SliceTable, RESIDU *Residu);

void enhance_layer_error_conceal(int layerId,MMO* pmmo,decoder_context* pdecoder_context,thread_context* pthread_context,int poc);

void base_layer_init(NAL* current_nal,int layer_id,MMO* pmmo,decoder_context* pdecoder_context,
							  int poc,int nal_ref_idc,LAYER_MMO* pcurrent_layer_mmo,
							  LIST_MMO* pcurr_pic,SPS* pcurr_sps,PPS* pcurr_pps);

void base_layer_error_conceal_non_key(int layer_id,MMO* pmmo,decoder_context* pdecoder_context,thread_context* pthread_context,
									  int poc,int nal_ref_idc,NAL* current_nal);
void base_layer_error_conceal_key(int layer_id,MMO* pmmo,decoder_context* pdecoder_context,thread_context* pthread_context,
									  int poc,int nal_ref_idc,NAL* current_nal);
void base_layer_error_conceal(int layer_id,MMO* pmmo,decoder_context* pdecoder_context,thread_context* pthread_context,
								  int poc,int nal_ref_idc);

#endif

