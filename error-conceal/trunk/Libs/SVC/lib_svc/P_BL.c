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
#include "type.h"
#include "symbol.h"
#include "decode_mb_P.h"




//SVC Files
#include "svc_type.h"
#include "BL.h"
#include "I_BL.h"
#include "P_BL.h"
#include "blk4x4.h"
#include "svc_data.h"
#include "Mb_P_svc.h"
#include "mb_padding.h"
#include "residual_4x4.h"
#include "residual_8x8.h"
#include "residual_chroma.h"
#include "svc_neighbourhood.h"
#include "motion_vector_svc.h"
#include "ResidualProcessing.h"
#include "mb_upsample_filter.h"
#include "CoeffLevelPrediction.h"
#include "InterLayerPrediction.h"










void Decode_BL_I(const PPS * Pps, const SPS * Sps, const NAL *Nal, RESIDU *residu, RESIDU *BaseLayerResidu, 
				 const W_TABLES *quantif, short x, short y, short x_base, short y_base, 
				 short x_mbPartIdx, short y_mbPartIdx, short PicWidthInPix, short mbBaseAddr, 
				 unsigned char Y[ ], unsigned char U[ ], unsigned char V[ ])
{

	
	const int base_offset_lum = 16 + 16*(Nal->base_Width+32);
	const int base_offset_cb = 8+4*(Nal->base_Width+32);
	unsigned char* base_luma = Nal->base_picture->picture_memory_y + base_offset_lum;
	unsigned char* base_cb = Nal->base_picture->picture_memory_u + base_offset_cb;
	unsigned char* base_cr = Nal->base_picture->picture_memory_v + base_offset_cb;

	RESIDU *BaseResidu = &BaseLayerResidu[mbBaseAddr];

	if ( !Nal -> SpatialScalability){
		//Bring back base layeer sample into current layer
		GetBaseSample(Y, U, V, &base_luma[((x + ( y * PicWidthInPix)) << 4)], 
			&base_cb[((x + (y * PicWidthInPix >> 1)) << 3)], 
			&base_cr[((x + (y * PicWidthInPix >> 1)) << 3)], PicWidthInPix);

		//We have to keep base and current layer rescale coefficient for Scoeff prediction
		//For upper layer
		if( residu -> Cbp){
			// In case of new SNR enhancement based on this one
			RescaleCurrentCoeff(residu, BaseResidu, Pps, quantif);
		}else if (Nal -> SCoeffPrediction){
			//mbl check
			//should be done with RescaleCurrentCoeff
			//we should be in SCoeffPrediction, no need to test.
			//===== modify QP values (as specified in G.8.1.5.1.2) =====
			//cbp == 0 && (Scoeff || Tcoeff) && (I_BL || ResidualPredictionFlag)
			residu -> Qp = BaseResidu -> Qp;
			residu -> Transform8x8 = BaseResidu -> Transform8x8;
		}
		
		//Test if cbp != 0 and if we are in the first enhancement layer in MGS or CGS
		if(BaseResidu -> MbType == I_BL){
			ComputeCurrentCoeff(Nal, residu, BaseResidu, Pps, quantif);
		}

		if ( Nal -> PicToDecode){
			if (residu -> Transform8x8){
				SCoeff8x8AddPic (Y, residu, PicWidthInPix);
			}else {
				SCoeff4x4AddPic (Y, residu, PicWidthInPix);
			}
			//Decode the chrominance
			SCoeffChromaAddPic(U, V, residu, PicWidthInPix >> 1);
		}

	}else {
		MbIPad(Nal, Pps, Sps, mbBaseAddr, x << 4, y << 4, x_base, y_base, x_mbPartIdx, y_mbPartIdx, 
			BaseLayerResidu,base_luma,base_cb,base_cr,  Y, U, V);

		if (Nal -> PicToDecode){
			//If we need to build the prediction ( for upper spatial enhancement
			if ( residu -> Cbp & 0x0f){
				if ( !residu -> Transform8x8){
					decode_P_Intra4x4_lum(Y, residu, PicWidthInPix, quantif -> W4x4_inter);
				} else {	
					decode_P_Intra8x8_lum(Y, residu, PicWidthInPix, quantif -> W8x8_inter);
				}
			}

			if ( residu -> Cbp > 0x0f){
				//Decode the chrominance
				decode_P_chroma(U, V, Pps, residu, PicWidthInPix >> 1, quantif);
			}
		}else {
			//In case of a SNR prediction in a upper layer.
			//We just rescale the coefficients.
			RescaleCurrentCoeff(residu, BaseResidu, Pps, quantif);
		}
	}

	residu -> blk4x4_Res = 0;
}






void Decode_P_BL(const PPS * Pps, const SPS * Sps, const NAL *Nal, RESIDU *CurrResidu, RESIDU *BaseResidu, 
				 const STRUCT_PF *baseline_vector, const W_TABLES *quantif, LIST_MMO *ai_pstRefPicListL0, 
				 short x, short y, short x_base, short y_base, short x_mbPartIdx, short y_mbPartIdx, short PicWidthInPix, 
				 REFPART *RefPartition, short ref_cache_l0[],  short mv_cache_l0 [][2], short *mv, short *ref_base, 
				 unsigned char Y[ ], unsigned char U[ ], unsigned char V[ ], 
				 short Residu_Luma[], short Residu_Cb[], short Residu_Cr[])
{



 	const short PicHeightInMbs = Sps -> pic_height_in_map_units + 2; 
	const short mbBaseAddr = x_base + y_base * (Nal -> base_Width >> 4);



	if ( IS_I_SVC(CurrResidu -> MbType)){
		const int offset_Lum = 16 + 16 * (Nal ->  base_Width + 32);
		const int offset_Cb = 8 + 4 * (Nal ->  base_Width + 32);
		unsigned char* base_luma = Nal->base_picture->picture_memory_y+ offset_Lum;
		unsigned char* base_cb = Nal->base_picture->picture_memory_u+ offset_Cb;
		unsigned char* base_cr = Nal->base_picture->picture_memory_v+ offset_Cb;

		ref_cache_l0[14] = ref_cache_l0[30] = -1;
		Decode_BL_I(Pps, Sps, Nal, CurrResidu, BaseResidu, quantif, x, y, x_base, y_base, 
			x_mbPartIdx, y_mbPartIdx, PicWidthInPix, mbBaseAddr, Y, U, V);

		ResetMbResidu(Residu_Luma, Residu_Cb, Residu_Cr, PicWidthInPix);

	}else{	
		if(!Nal -> non_diadic){
			//Inter prediction in diadic mode
			//So we are sure that 's all macroblock is predicted in inter mode
			int offset_Lum = 16 + 16 * (Nal -> curr_Width + 32);
			int offset_Cb = 8 + 4 * (Nal ->  curr_Width + 32);
			unsigned char* curr_luma = Nal->current_picture->picture_memory_y + offset_Lum;
			unsigned char* curr_cb = Nal->current_picture->picture_memory_u + offset_Cb;
			unsigned char* curr_cr = Nal->current_picture->picture_memory_v + offset_Cb;


			get_base_P_dyadic(mv_cache_l0, ref_cache_l0, curr_luma, curr_cb, curr_cr, 
				Y, U, V, x << 4, y << 4, PicWidthInPix, PicHeightInMbs << 4, ai_pstRefPicListL0, 
				mv, ref_base, RefPartition, Nal, baseline_vector -> Ptr_interpol_4x4);

			AddResidual(Nal, Pps, CurrResidu, &BaseResidu[mbBaseAddr], PicWidthInPix, quantif, Y, U, V, Residu_Luma, Residu_Cb, Residu_Cr);

		}else {
			short Y_Res[256];
			short U_Res[64];
			short V_Res[64];
			unsigned char MbPredY[32*32];
			unsigned char MbPredU[256];
			unsigned char MbPredV[256];


			//Inter prediction in diadic mode
			//The macroblock  can be inter and intra predicted.
			int cX = 8; 
			int cY = 8; 

			int offset_Lum = 16 + 16 * (Nal -> curr_Width + 32);
			int offset_Cb = 8 + 8 * (Nal ->  curr_Width + 16);
			int BaseoffsetLum = 16 + 16 * (Nal ->  base_Width + 32);
			int BaseOffsetCb = 8 + 8 * (Nal ->  base_Width + 16);

			short one_I = (RefPartition[0] . RefIdc == -2) || (RefPartition[2] . RefIdc == -2) || (RefPartition[8] . RefIdc == -2) || (RefPartition[10] . RefIdc == -2);

			//Calculate the residual
			if ( !CurrResidu -> Transform8x8){
				Intra4x4_lum(Y_Res, CurrResidu, 16, quantif -> W4x4_inter);
			} else {
				Intra8x8_lum(Y_Res, CurrResidu, 16, quantif -> W8x8_inter);
			}

			//Decode the chrominance
			decode_chroma(U_Res, V_Res, CurrResidu, Pps, 8, quantif);


			if( one_I){ 
				DecodeIPart(Nal, Pps, Sps, RefPartition, BaseResidu, PicWidthInPix, mbBaseAddr, x << 4, y << 4, 
					x_base, y_base, x_mbPartIdx, y_mbPartIdx, Nal->base_picture->picture_memory_y+ BaseoffsetLum, 
					Nal->base_picture->picture_memory_u + BaseOffsetCb, 
					Nal->base_picture->picture_memory_v + BaseOffsetCb, 
					Y, U, V, MbPredY, MbPredU, MbPredV);
			}

			// Get inter interpolation
			get_base_P_non_dyadic(mv_cache_l0, ref_cache_l0	, Y, U, V, x << 4, y << 4, PicWidthInPix, PicHeightInMbs << 4, ai_pstRefPicListL0, ref_base, 
				Nal, baseline_vector -> Ptr_interpol_4x4, RefPartition);


			//First block 8x8
			GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[0] . RefIdc, 8, 8, 
				PicWidthInPix, Y, MbPredY, Residu_Luma, Y_Res);
			GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[0] . RefIdc, 4, 4, 
				PicWidthInPix >> 1, U, MbPredU, Residu_Cb, U_Res);
			GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[0] . RefIdc, 4, 4, 
				PicWidthInPix >> 1, V, MbPredV, Residu_Cr, V_Res);

			//Second block 8x8
			GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[2] . RefIdc, 8, 8, 
				PicWidthInPix, &Y[cX], &MbPredY[cX], &Residu_Luma[cX], &Y_Res[cX]);
			GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[2] . RefIdc, 4, 4, 
				PicWidthInPix >> 1, &U[cX >> 1], &MbPredU[cX >> 1], &Residu_Cb[cX >> 1], &U_Res[cX >> 1]);
			GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[2] . RefIdc, 4, 4, 
				PicWidthInPix >> 1, &V[cX >> 1], &MbPredV[cX >> 1], &Residu_Cr[cX >> 1], &V_Res[cX >> 1]);

			//Third block 8x8
			GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[8] . RefIdc, 8, 8, 
				PicWidthInPix, &Y[cY * PicWidthInPix], &MbPredY[cY << 4], &Residu_Luma[cY * PicWidthInPix], &Y_Res[cY << 4]);
			GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[8] . RefIdc, 4, 4, 
				PicWidthInPix >> 1, &U[(cY >> 1) * (PicWidthInPix >> 1)], &MbPredU[cY << 2], &Residu_Cb[(cY >> 1) * (PicWidthInPix >> 1)], &U_Res[cY << 2]);
			GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[8] . RefIdc, 4, 4, 
				PicWidthInPix >> 1, &V[(cY >> 1) * (PicWidthInPix >> 1)], &MbPredV[cY << 2], &Residu_Cr[(cY >> 1) * (PicWidthInPix >> 1)], &V_Res[cY << 2]);


			//Fourth block 8x8
			GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[10] . RefIdc, 8, 8, 
				PicWidthInPix, &Y[cX + cY * PicWidthInPix], &MbPredY[cX + (cY << 4)], &Residu_Luma[cX + cY * PicWidthInPix], &Y_Res[cX + (cY << 4)]);
			GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[10] . RefIdc, 4, 4, 
				PicWidthInPix >> 1, &U[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &MbPredU[(cX >> 1) + (cY << 2)], 
				&Residu_Cb[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &U_Res[(cX >> 1) + (cY << 2)]);
			GetNonDiadicSample(CurrResidu -> ResidualPredictionFlag, RefPartition[10] . RefIdc, 4, 4, 
				PicWidthInPix >> 1, &V[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &MbPredV[(cX >> 1) + (cY << 2)], 
				&Residu_Cr[(cX >> 1) + (cY >> 1) * (PicWidthInPix >> 1)], &V_Res[(cX >> 1) + (cY << 2)]);

			//Update the bl4x4 flag in order to know if the 4x4 block are intra coded.
			update_blk4x4Res(Y_Res, CurrResidu, 16);
		}
	}
}
