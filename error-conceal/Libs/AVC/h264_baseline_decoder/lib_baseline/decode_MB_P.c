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

#define C_MB_P
#include "data.h"
#include "clip.h"
#include "Transform.h"
#include "decode_mb_P.h"
#include "quantization.h"
#include "neighbourhood.h"
#include "transform_8x8.h"
#include "motion_vector.h"
#include "interpolation.h"
#include "interpolation_8x8.h"



/**
   This function permits to decode all intra4x4 luminance blocks in a MB.

@param image Table of current frame.
@param LumaLevel Contains the AC luma level of a 4x4 decoded macroblock.
@param PicWidthInPix Width in pixels of the current frame.
@param residu Structure which contains all the residu data of the current macroblock.
@param W4x4_inter is the scaling matrix.
*/
void decode_P_Intra4x4_lum ( unsigned char image [], const RESIDU *residu, const short PicWidthInPix,  const short W4x4_inter [6][16])
{
    int index ;
	short i8x8 ;
    short luma_coeff [64];
    
    //Boucle sur chaque block 
    for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {
        
        if (residu -> Cbp & (1 << i8x8))   {    
			/* each luma 8x8 block */
            short locx = ((i8x8 & 1) << 3);
            short locy = ((i8x8 & 2) << 2);
            
            /* each 4x4 sub-block of block */
            index = i8x8 << 6;
            rescale_residual(residu -> Qp, &residu -> LumaLevel [index], luma_coeff, W4x4_inter);
            ict_4x4_residual(luma_coeff, &image[locx + locy * PicWidthInPix],  PicWidthInPix);
            ict_4x4_residual(&luma_coeff[16], &image[locx + 4 + locy * PicWidthInPix],  PicWidthInPix);
            ict_4x4_residual(&luma_coeff[32], &image[locx + (locy + 4) * PicWidthInPix],  PicWidthInPix);
            ict_4x4_residual(&luma_coeff[48], &image[4 + locx + (locy + 4) * PicWidthInPix],  PicWidthInPix);
        }
    }
}



/**
   This function permits to decode all intra8x8 luminance blocks in a MB.

@param image Table of current frame.
@param LumaLevel Contains the AC luma level of a 4x4 decoded macroblock.
@param PicWidthInPix Width in pixels of the current frame.
@param residu Structure which contains all the residu data of the current macroblock.
@param W8x8_inter is the scaling matrix.
*/
void decode_P_Intra8x8_lum ( unsigned char image [], const RESIDU *residu, const short PicWidthInPix, const short W8x8_inter [6][64])
{
    int i8x8 ;
    short luma_coeff [64];
    
    //Boucle sur chaque block 
    for ( i8x8 = 0 ; i8x8 < 4 ; i8x8++ ) {        
		if (residu -> Cbp & (1 << i8x8)){

			/* each luma 8x8 block */
            int locx = (( i8x8 & 1) << 3);
            int locy = (( i8x8 & 2) << 2);
            
            rescale_8x8_residual(residu -> Qp, &residu -> LumaLevel [i8x8 << 6], luma_coeff, W8x8_inter);
            ict_8x8(luma_coeff, &image[locx + locy * PicWidthInPix], PicWidthInPix );
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
void decode_P_chroma (unsigned char image_Cb [], unsigned char image_Cr [], const PPS *pps, const RESIDU *residu, const short PicWidthInPix, const W_TABLES *quantif)
{

    short DC_coeff [4];
    short AC_coeff [16];
   

    //for Cb
	if ( residu -> Cbp & 0x30){
		//Initialisation du QP 
		int QPc = qP_tab [residu -> Qp + pps -> chroma_qp_index_offset];
		ict_4x4_dc_chroma(&residu -> ChromaDCLevel_Cb [0], DC_coeff);
		rescale_4x4_dc_chr(QPc,  DC_coeff, DC_coeff, quantif -> W4x4_Cb_inter);
	
		rescale_4x4_residual(QPc,  &residu -> ChromaACLevel_Cb [0], AC_coeff, quantif -> W4x4_Cb_inter);
		AC_coeff [0] = DC_coeff [0];
		ict_4x4_residual(AC_coeff, image_Cb, PicWidthInPix);
		rescale_4x4_residual(QPc,  &residu -> ChromaACLevel_Cb [16], AC_coeff,quantif -> W4x4_Cb_inter);
		AC_coeff [0] = DC_coeff [1];
		ict_4x4_residual(AC_coeff, &image_Cb[4],PicWidthInPix);
		rescale_4x4_residual(QPc,  &residu -> ChromaACLevel_Cb [32], AC_coeff, quantif -> W4x4_Cb_inter);
		AC_coeff [0] = DC_coeff [2];
		ict_4x4_residual(AC_coeff, &image_Cb[PicWidthInPix << 2],PicWidthInPix);
		rescale_4x4_residual(QPc,  &residu -> ChromaACLevel_Cb [48], AC_coeff, quantif -> W4x4_Cb_inter);
		AC_coeff [0] = DC_coeff [3];
		ict_4x4_residual(AC_coeff, &image_Cb[(1 + PicWidthInPix) << 2],PicWidthInPix);

		QPc = qP_tab [residu -> Qp + pps -> second_chroma_qp_index_offset];
		ict_4x4_dc_chroma(&residu -> ChromaDCLevel_Cr [0], DC_coeff);
		rescale_4x4_dc_chr(QPc,  DC_coeff, DC_coeff, quantif -> W4x4_Cr_inter);


		rescale_4x4_residual(QPc,  &residu -> ChromaACLevel_Cr [0], AC_coeff, quantif -> W4x4_Cr_inter);
		AC_coeff [0] = DC_coeff [0];
		ict_4x4_residual(AC_coeff, image_Cr,PicWidthInPix);
		rescale_4x4_residual(QPc,  &residu -> ChromaACLevel_Cr [16], AC_coeff, quantif -> W4x4_Cr_inter);
		AC_coeff [0] = DC_coeff [1];
		ict_4x4_residual(AC_coeff, &image_Cr[4],PicWidthInPix);
		rescale_4x4_residual(QPc,  &residu -> ChromaACLevel_Cr [32], AC_coeff, quantif -> W4x4_Cr_inter);
		AC_coeff [0] = DC_coeff [2];
		ict_4x4_residual(AC_coeff, &image_Cr[PicWidthInPix << 2],PicWidthInPix);
		rescale_4x4_residual(QPc,  &residu -> ChromaACLevel_Cr [48], AC_coeff, quantif -> W4x4_Cr_inter);
		AC_coeff [0] = DC_coeff [3];
		ict_4x4_residual(AC_coeff, &image_Cr[(1 + PicWidthInPix) << 2],PicWidthInPix);
	}
}




