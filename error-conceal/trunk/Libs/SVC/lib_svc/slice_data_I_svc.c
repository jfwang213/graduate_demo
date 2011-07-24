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


#include <stdio.h>
#include <string.h>

//AVC files
#include "type.h"
#include "neighbourhood.h"
#include "SliceGroupMap.h"


//SVC files
#include "svc_data.h"
#include "Mb_I_svc.h"
#include "Loop_filter_ext.h"
#include "slice_data_I_svc.h"
#include "svc_neighbourhood.h"
#include "InterLayerPrediction.h"




void Decode_I_svc ( const int size_Mb , const SPS *ai_pstSps, const PPS *ai_pstPps, const SLICE *Slice, 
				   const NAL * Nal,  DATA *Block, RESIDU *curr_residu, 
				   STRUCT_PF *baseline_vector, LIST_MMO *Current_pic, const W_TABLES *quantif,MMO *mmo)
{



	if (!Nal -> NalToSkip){
		LAYER_MMO* pLayerMmo = &(mmo->LayerMMO[Nal->LayerId]);
		RESIDU *base_residu = mmo->LayerMMO[Nal->BaseLayerId].residu;

		int pic_parameter_id = Nal -> pic_parameter_id[Nal -> LayerId];
		const PPS *Pps = &ai_pstPps[pic_parameter_id];
		const SPS *Sps = &ai_pstSps[Pps -> seq_parameter_set_id + 16];

		short iCurrMbAddr = Slice -> first_mb_in_slice ;
		const short PicSizeInMbs = Sps -> PicSizeInMbs ;
		const short PicWiInMbs = Sps -> pic_width_in_mbs;
		const short PicWidthInMbs = Sps -> pic_width_in_mbs + 2;
		const short PicWidthInPix = PicWidthInMbs * 16 ;
		const short offset_L = ( PicWidthInMbs << 8) + 16;
		const short offset_C = ( PicWidthInMbs << 6) + 8;  


		unsigned char *Y = offset_L + Current_pic -> picture_memory_y;
		unsigned char *U = offset_C + Current_pic->picture_memory_u;
		unsigned char *V = offset_C + Current_pic->picture_memory_v;
		unsigned char *BaseY = Nal->base_picture->picture_memory_y + 16 + ((Nal -> base_Width + 32) << 4);
		unsigned char *BaseU = Nal->base_picture->picture_memory_u + 8 + ((Nal -> base_Width + 32) << 2);
		unsigned char *BaseV = Nal->base_picture->picture_memory_v + 8 + ((Nal -> base_Width + 32) << 2);

 
		short iMb_x;
		short iMb_y;
		
		//initialize macroblock position
		GetMbPosition(iCurrMbAddr, PicWiInMbs, &iMb_x, &iMb_y);


		GetIBasePrediction(Nal, Slice, ai_pstPps, ai_pstSps, Sps, base_residu, 
			BaseY, BaseU, BaseV, Y, U, V);
			
			
		//Loop on all macroblocks
		for ( ; iCurrMbAddr < PicSizeInMbs ;) {

			if (IS_I(curr_residu [iCurrMbAddr] . MbType) || !Nal -> SpatialScalability) {
				//For all SNR enhancement
				// I macroblock, or Scoeff and Tcoeff prediction are treated
				GetNeighbour(iCurrMbAddr, iMb_x, PicWiInMbs, PicSizeInMbs, pLayerMmo->mb_slice_table, curr_residu, Pps -> constrained_intra_pred_flag);

				MbISnr(Pps, Nal, quantif, baseline_vector, &curr_residu [iCurrMbAddr], &base_residu [iCurrMbAddr], PicWiInMbs, 
					&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], &V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)]);

			}else{
				MbISpatial (Nal, Sps, Pps, PicWidthInPix, quantif, iMb_x, iMb_y, &Block [iCurrMbAddr], &curr_residu [iCurrMbAddr], base_residu, 
					&Y[((iMb_x + (iMb_y * PicWidthInPix)) << 4)], &U[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], 
					&V[((iMb_x + (iMb_y * PicWidthInPix >> 1)) << 3)], BaseY, BaseU, BaseV);
			}

			//Updating the macroblock address
			iCurrMbAddr = Next_MbAddress(iCurrMbAddr, PicSizeInMbs, PicWiInMbs, pLayerMmo->mb_slice_table, &iMb_x, &iMb_y);
			//Used to stop the deconding process when there is slices.
			if ( pLayerMmo->mb_slice_table [iCurrMbAddr] == 255 ) {
				iCurrMbAddr = PicSizeInMbs ;
			}
		}
	}
}


