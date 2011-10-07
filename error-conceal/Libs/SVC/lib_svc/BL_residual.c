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

#include "string.h"

//AVC Files
#include "type.h"
#include "symbol.h"
#include "Transform.h"
#include "decode_mb_P.h"
#include "transform_8x8.h"
#include "quantization.h"



//SVC Files
#include "svc_type.h"
#include "BL_residual.h"
#include "CoeffLevelPrediction.h"



/**
This function allows to decode an intraBL luminance MB with 4x4 prediction.

@param Y Table of current frame.
@param PicWidthInMbs Width in macroblocks of the current frame.
*/

void decode_Intra4x4_BL_lum ( unsigned char *Y, RESIDU *CurrResidu, const short PicWidthInPix, const short W4x4_intra [6][16]){

	short i ;

	// each luma 4x4 block
	for ( i = 0 ; i < 16 ; i++ )  {       
		// Update prediction and compute values
		if (CurrResidu -> Cbp & 1 << ((i & 12) >> 2)) {
			short Locx = LOCX(i);
			short Locy = LOCY(i);

			rescale_4x4_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [i << 4], &CurrResidu -> LumaLevel [i << 4], W4x4_intra);
			ict_4x4_residual(&CurrResidu -> LumaLevel [i << 4], &Y[Locx + Locy * PicWidthInPix], PicWidthInPix);
		}
	}
}


/**
This function allows to decode an intraBL luminance MB with 8x8 prediction.

@param Y Table of current frame.
@param PicWidthInMbs Width in macroblocks of the current frame.
*/
void decode_Intra8x8_BL_lum ( unsigned char *Y, RESIDU *CurrResidu, const short PicWidthInPix, const short W8x8_intra [6][64] )
{

	int i;

	for ( i = 0 ; i < 4 ; i++ ) 
	{       
		int Locx = LOCX(i << 2);
		int Locy = LOCY(i << 2);

		// Update the prediction
		if (CurrResidu -> Cbp & 1 << i)  {
			rescale_8x8_residual(CurrResidu -> Qp, &CurrResidu -> LumaLevel [i << 6], &CurrResidu -> LumaLevel [i << 6], W8x8_intra);
			ict_8x8(&CurrResidu -> LumaLevel [i << 6], &Y[Locx +  Locy * PicWidthInPix], PicWidthInPix);
		}
	}
}



/**
This function permits to decode all intra chrominances in a MB.

@param image_Cb Table of current frame.
@param image_Cr Table of current frame.
@param Block Data of the current macroblock.
@param PicWidth Width in macroblocks of the current frame.
@param pps PPS structure of the current video.
@param macro Specifies the availability of the neighbouring macroblock.
@param residu Structure which contains all the residu data of the current macroblock..
@param quantif Table which contains the quantization table.
@param mb4x4_mode is the function vector for intra prediction
*/
void DecodeChromaBL (unsigned char U [], unsigned char V [], const PPS *pps, RESIDU *residu, const short PicWidthInPix, const W_TABLES *quantif)
{

	short DC_coeff [4];

	//for Cb
	if ( residu -> Cbp & 0x30){
		//Initialisation du QP 
		int QPc = qP_tab [residu -> Qp + pps -> chroma_qp_index_offset];
		ict_4x4_dc_chroma(&residu -> ChromaDCLevel_Cb [0], DC_coeff);
		rescale_4x4_dc_chr(QPc, DC_coeff, DC_coeff, quantif -> W4x4_Cb_inter);

		rescale_4x4_residual(QPc, residu -> ChromaACLevel_Cb, residu -> ChromaACLevel_Cb, quantif -> W4x4_Cb_inter);
		residu -> ChromaACLevel_Cb [0] = DC_coeff [0];
		ict_4x4_residual(residu -> ChromaACLevel_Cb, U, PicWidthInPix);

		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cb [16], &residu -> ChromaACLevel_Cb [16], quantif -> W4x4_Cb_inter);
		residu -> ChromaACLevel_Cb [16] = DC_coeff [1];
		ict_4x4_residual(&residu -> ChromaACLevel_Cb [16], &U[4], PicWidthInPix);

		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cb [32], &residu -> ChromaACLevel_Cb [32], quantif -> W4x4_Cb_inter);
		residu -> ChromaACLevel_Cb [32] = DC_coeff [2];
		ict_4x4_residual(&residu -> ChromaACLevel_Cb [32], &U[PicWidthInPix << 2], PicWidthInPix);

		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cb [48], &residu -> ChromaACLevel_Cb [48], quantif -> W4x4_Cb_inter);
		residu -> ChromaACLevel_Cb [48] = DC_coeff [3];
		ict_4x4_residual(&residu -> ChromaACLevel_Cb [48], &U[(1 + PicWidthInPix) << 2], PicWidthInPix);

		QPc = qP_tab [residu -> Qp + pps -> second_chroma_qp_index_offset];
		ict_4x4_dc_chroma(&residu -> ChromaDCLevel_Cr [0], DC_coeff);
		rescale_4x4_dc_chr(QPc,  DC_coeff, DC_coeff, quantif -> W4x4_Cr_inter);


		rescale_4x4_residual(QPc, residu -> ChromaACLevel_Cr, residu -> ChromaACLevel_Cr, quantif -> W4x4_Cr_inter);
		residu -> ChromaACLevel_Cr [0] = DC_coeff [0];
		ict_4x4_residual(residu -> ChromaACLevel_Cr, V, PicWidthInPix);

		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cr [16], &residu -> ChromaACLevel_Cr [16], quantif -> W4x4_Cr_inter);
		residu -> ChromaACLevel_Cr [16] = DC_coeff [1];
		ict_4x4_residual(&residu -> ChromaACLevel_Cr [16], &V[4], PicWidthInPix);

		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cr [32], &residu -> ChromaACLevel_Cr [32], quantif -> W4x4_Cr_inter);
		residu -> ChromaACLevel_Cr [32] = DC_coeff [2];
		ict_4x4_residual(&residu -> ChromaACLevel_Cr [32], &V[PicWidthInPix << 2], PicWidthInPix);

		rescale_4x4_residual(QPc, &residu -> ChromaACLevel_Cr [48], &residu -> ChromaACLevel_Cr [48], quantif -> W4x4_Cr_inter);
		residu -> ChromaACLevel_Cr [48] = DC_coeff [3];
		ict_4x4_residual(&residu -> ChromaACLevel_Cr [48], &V[(1 + PicWidthInPix) << 2], PicWidthInPix);
	}
}






/**
Compute the residual for a I macroblock.
*/
void ComputeCurrentResidual(const NAL *Nal, RESIDU *CurrResidu, RESIDU *BaseResidu, const PPS *pps, const W_TABLES *Quantif, 
							const short PicWidthInPix, unsigned char *Y, unsigned char *U, unsigned char *V)
{

	//If we have to reconstruct the frame, in case of spatial enhancement is based on this layer.
	if (Nal -> PicToDecode){
		if ( CurrResidu -> Cbp){
			if (!CurrResidu -> Transform8x8 ){
				decode_Intra4x4_BL_lum(Y, CurrResidu, PicWidthInPix, Quantif -> W4x4_inter);
			} else {
				decode_Intra8x8_BL_lum(Y, CurrResidu, PicWidthInPix, Quantif -> W8x8_inter);
			}

			// Add residual to prediction for chrominance
			decode_P_chroma(U, V, pps, CurrResidu, PicWidthInPix >> 1, Quantif);
		}
	}else {
		if( CurrResidu -> Cbp){
			// In case of new SNR enhancement based on this one
			RescaleCurrentCoeff(CurrResidu, BaseResidu, pps, Quantif);
		}
	}
}
