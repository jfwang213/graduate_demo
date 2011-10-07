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
#include <string.h>


//AVC Files
#include "data.h"

//SVC Files
#include "svc_type.h"
#include "upsample_residu.h"
#include "Loop_filter_ext.h"
#include "InterLayerPrediction.h"




/**
This function copies for SNR layers, base layer samples into current layer.
*/
void GetBaseSample(unsigned char *Y, unsigned char *U, unsigned char *V, unsigned char *aio_tucDpb_luma, 
				   unsigned char *aio_tucDpb_Cb, unsigned char *aio_tucDpb_Cr, short PicWidthInPix)
{
	int i;
	for(i = 0; i < 16; i++){
		memcpy(&Y[i * PicWidthInPix], &aio_tucDpb_luma[i * PicWidthInPix], 16 * sizeof(char));
	}

	PicWidthInPix >>= 1;
	for(i = 0; i < 8; i++){
		memcpy(&U[i * PicWidthInPix], &aio_tucDpb_Cb[i * PicWidthInPix], 8 * sizeof(char));
		memcpy(&V[i * PicWidthInPix], &aio_tucDpb_Cr[i * PicWidthInPix], 8 * sizeof(char));
	}
}






/**
This function applies or not the interlayer filter if necessary.
*/
void GetBaseFilter(const NAL *Nal, const SLICE *Slice, const PPS *Pps, const SPS *Sps,  RESIDU *BaseResidu, 
				   unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV)
{
	//Inter layer deblocking filter if necessary
	if (Slice -> slice_num == 0 && (Slice -> InterLayerDeblockingFilterIdc != 1 || Nal -> LoopFilterForcedIdc)){
		int base_pic_parameter_id = Nal -> pic_parameter_id[Nal -> BaseLayerId];
		int base_seq_parameter_id = Pps[base_pic_parameter_id] . seq_parameter_set_id + (Nal -> BaseLayerId ? 16: 0); 
		Deblocking_base_layer(&Sps[base_seq_parameter_id], &Pps[Nal -> BaseLayerId], BaseResidu, 
			Nal, Slice -> InterLayerDeblockingFilterIdc, BaseY, BaseU, BaseV);
	}
}




/**
This function allow to deblock the base layer if it's a spatial enhancement, and to upsample the residu
for  B and P slices.
*/
void GetBasePrediction(const NAL *Nal, const SLICE *Slice, const PPS *Pps, const SPS *Sps, const SPS *Sps_id, 
					   RESIDU *CurrResidu, RESIDU *BaseResidu, const int size_Mb, short iCurrMbAddr, 
					   const unsigned char *ai_tiMbToSliceGroupMap, short *x16, short *y16, short *xk16, 
					   short *xp16, short *yk16, short *yp16, short *Upsampling_tmp, 
					   unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV)
{



	short *Base_Residu_Luma = Nal->base_picture->residual_memory_y; 
	short *Base_Residu_Cb = Nal->base_picture->residual_memory_u;
	short *Base_Residu_Cr = Nal->base_picture->residual_memory_v; 

	short *Residu_Luma = Nal->current_picture->residual_memory_y;
	short *Residu_Cb =  Nal->current_picture->residual_memory_u;
	short *Residu_Cr =  Nal->current_picture->residual_memory_v;

	int nb_val = (Sps_id -> pic_width_in_mbs + 2) * (Sps_id -> pic_height_in_map_units+2) * 256;

	if (!Nal -> no_inter_layer_pred_flag &&  Nal->base_picture != NULL){
		GetBaseFilter(Nal, Slice, Pps, Sps, BaseResidu, BaseY, BaseU, BaseV);

		if (Nal -> SpatialScalability){
			if (Slice -> slice_num == 0){
				//Reset current residu
				memset(Residu_Luma, 0, nb_val * sizeof(short));
				memset(Residu_Cb, 0, (nb_val >> 2) * sizeof(short));
				memset(Residu_Cr, 0, (nb_val >> 2) * sizeof(short));
			}
			// Just upsample the CurrResidual.
			Upsample_residu( iCurrMbAddr, Nal, Sps_id, CurrResidu, 
				BaseResidu, x16, y16, xk16, xp16, yk16, yp16, Upsampling_tmp, 
				Base_Residu_Luma, Base_Residu_Cb, Base_Residu_Cr, ai_tiMbToSliceGroupMap,Residu_Luma,
				Residu_Cb,Residu_Cr);

		}else if (Slice -> slice_num == 0){
			//Copy the base layer residu into the one of the current layer.
			memcpy(Residu_Luma, Base_Residu_Luma, nb_val * sizeof(short));
			memcpy(Residu_Cb, Base_Residu_Cb, nb_val * sizeof(short) >> 2);
			memcpy(Residu_Cr, Base_Residu_Cr, nb_val * sizeof(short) >> 2);	
		}
	} else if (Slice -> slice_num == 0){
		//Reset the layer in order not to corrumpt the decoding process.
		memset(Residu_Luma, 0, nb_val * sizeof(short));
		memset(Residu_Cb, 0, nb_val * sizeof(short) >> 2);
		memset(Residu_Cr, 0, nb_val * sizeof(short) >> 2);	
	}
}




/**
This function allow to deblock the base layer if it's a spatial enhancement, and to upsample the residu
for I slices.
*/
void GetIBasePrediction(const NAL *Nal, const SLICE *Slice, const PPS *Pps, const SPS *Sps, const SPS *Sps_id, RESIDU *BaseResidu, 
				   unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV,
				   unsigned char *Y, unsigned char *U, unsigned char *V)
{
	GetBaseFilter(Nal, Slice, Pps, Sps, BaseResidu, BaseY, BaseU, BaseV);

	if (!Nal -> SpatialScalability && !Slice -> slice_num){
		//Copy base layer into currentlayer.
		int nb_val = (Sps_id -> pic_width_in_mbs + 2) * Sps_id -> pic_height_in_map_units * 256;
		memcpy(Y, BaseY, nb_val * sizeof(char));
		memcpy(U, BaseU, nb_val * sizeof(char) >> 2);
		memcpy(V, BaseV, nb_val * sizeof(char) >> 2);	
	}
}
