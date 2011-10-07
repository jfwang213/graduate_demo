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



//AVC files
#include "type.h"
#include "symbol.h"
#include "decode_mb_I.h"
#include "decode_mb_P.h"
#include "neighbourhood.h"


//SVC files
#include "svc_type.h"
#include "svc_data.h"
#include "I_BL.h"
#include "Mb_I_svc.h"
#include "BL_residual.h"
#include "Coordonates.h"
#include "ResidualProcessing.h"
#include "CoeffLevelPrediction.h"




/**
Decode a I_BL marcoblock using SNR prediction.
*/
void MbISnr ( const PPS *Pps, const NAL* Nal, const W_TABLES *Quantif, const STRUCT_PF *BaselineVectors, 
			 RESIDU *CurrResidu , RESIDU *BaseResidu, const short PicWidthInMbs, 
			 unsigned char *Y, unsigned char *U, unsigned char *V)
{

	const short PicWidthInPix = (PicWidthInMbs + 2) << 4;

	//Construction process for one macroblock
	if (IS_I(CurrResidu -> MbType)){
		//I type macroblock decoding process
		decode_MB_I(Y, U, V, Pps, CurrResidu, PicWidthInMbs + 2, Quantif, BaselineVectors );

	}else if (Nal -> TCoeffPrediction){
		//For SNR layer, with Tcoeff level prediction
		//Initialization of the macroblock neighbourhood

		//Use base macroblock to determine current macroblock parameters.
		CurrResidu -> MbType = BaseResidu -> MbType;
		CurrResidu -> Intra16x16PredMode = BaseResidu -> Intra16x16PredMode;
		CurrResidu -> IntraChromaPredMode = BaseResidu -> IntraChromaPredMode;

		//Decoding proces of the macroblock, as an INTRA_BL
		Decode_I_BL(CurrResidu, BaseResidu, Pps, PicWidthInMbs + 2, Quantif, BaselineVectors, Y, U, V);

	}else if (Nal -> SCoeffPrediction){
		if (!CurrResidu -> Cbp){
			//===== modify QP values (as specified in G.8.1.5.1.2) =====
			//cbp == 0 && (Scoeff || Tcoeff) && (I_BL || ResidualPredictionFlag)
			CurrResidu -> Qp = BaseResidu -> Qp;
		}
		if ((CurrResidu -> Cbp & 0x0f) == 0){
			CurrResidu -> Transform8x8 = BaseResidu -> Transform8x8;
		}

		if ( IS_BL (BaseResidu -> MbType)){
			//In case of two SNR enchancement
			//We have to get the zero quality prediction
			//And to add to this all Scoeff 
			SCoeffResidualAddPic(Nal, CurrResidu, BaseResidu, Pps, PicWidthInPix, Quantif, Y, U, V);
		}else if (Nal -> PicToDecode){
			//We have only one layer prediction, so we can jsut add to it, the residual.
			if (!CurrResidu -> Transform8x8 ){
				decode_Intra4x4_BL_lum(Y, CurrResidu, PicWidthInPix, Quantif -> W4x4_inter);
			} else {
				decode_Intra8x8_BL_lum(Y, CurrResidu, PicWidthInPix, Quantif -> W8x8_inter);
			}

			/// Add residual to prediction for chrominance
			DecodeChromaBL(U, V, Pps , CurrResidu, PicWidthInPix >> 1, Quantif);	
		}else{
			if( CurrResidu -> Cbp){
				// In case of new SNR enhancement based on this one
				RescaleCurrentCoeff(CurrResidu, BaseResidu, Pps, Quantif);
			}
		}
	}
}




/**
Decode a I_BL marcoblock using spatial prediction.
*/
void MbISpatial (const NAL *Nal, const SPS *Sps, const PPS *Pps, const short PicWidthInPix, const W_TABLES *quantif, 
				 short x, short y, DATA *CurrBlock, RESIDU *CurrResidu, RESIDU *BaseResidu, 
				 unsigned char *Y, unsigned char *U, unsigned char *V, 
				 unsigned char *BaseY, unsigned char *BaseU, unsigned char *BaseV)
{


	// For all other case, spatial layer
	short x_base, y_base, x_mbPartIdx, y_mbPartIdx;
	
	//Get coordonate in the lower layer
	short BaseMbAddr = CurrBlock -> MbBaseAddr;
	GetBaseMbCoord(CurrBlock, &x_base, &y_base, &x_mbPartIdx, &y_mbPartIdx);
	
	//Get the prediction by upsampling
	MbIPad(Nal, Pps, Sps, BaseMbAddr, x << 4, y << 4, x_base, y_base, x_mbPartIdx, y_mbPartIdx, BaseResidu, BaseY, BaseU, BaseV, Y, U, V);

	//Compute residual
	ComputeCurrentResidual(Nal, CurrResidu, BaseResidu, Pps, quantif, PicWidthInPix, Y, U, V);
}
