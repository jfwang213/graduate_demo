/*****************************************************************************
*
*  Open SVC Decoder developped in IETR image lab
*
*
*
*              Médéric BLESTEL <mblestel@insa-rennes.Fr>
*              Mickael RAULET <mraulet@insa-rennes.Fr>
*			  Maxime  PELCAT <mpelcat@insa-rennes.Fr>
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
* but WITHOUT ANY WARRANTY; without even the implied warranty off
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



#include "data.h"
#include "cavlc.h"
#include "mb_pred.h"
#include "bitstream.h"
#include "fill_caches.h"
#include "macroblock_partitionning.h"





void ParseIPCM(const unsigned char *ai_pcData, int *aio_piPosition, RESIDU *picture_residu, unsigned char *aio_tiNon_zero_count_cache){

	int i;

	for ( i = 0 ; i < 256 /** ChromaFormatFactor */; i++ ) {
		picture_residu -> LumaLevel[i] = (short) getNbits(ai_pcData, aio_piPosition, 8);
	}

	for ( i = 0 ; i < 64 /** ChromaFormatFactor */; i++ ) {
		picture_residu -> ChromaACLevel_Cb[i] = (short) getNbits(ai_pcData, aio_piPosition, 8);
	}

	for ( i = 0 ; i < 64 /** ChromaFormatFactor */; i++ ) {
		picture_residu -> ChromaACLevel_Cr[i] = (short) getNbits(ai_pcData, aio_piPosition, 8);
	}

	init_mb_IPCM(aio_tiNon_zero_count_cache);
	picture_residu -> Qp = 0;


}

/**
This function permits to recover the macroblock's data from the vlc
All the parameters decoded will be stored in differents structures or tables.

@param Pps PPS structure of the current video.
@param picture_residu Structure whixh contains information about the macroblock.
@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param aio_pstSlice The aio_pstSlice structure.
@param block Contains all parameters of the current macroblock.
@param vlc The VLC tables in order to decode the Nal Unit.
@param non_zero_count_cache Specifies the coeff_token of each blocks 4x4 of a macroblock.
@param non_zero_count Specifies the coeff_token of each block of the picture.
@param aio_pstSlice_table Specifies in which aio_pstSlice belongs each macroblock of the picture.
@param intra_pred_mod Contains the prediction mode for each macroblock.
@param ai_iMb_x The x position of the macroblock in the picture.
@param ai_iMb_y The y position of the macroblock in the picture.
@param last_QP Give the QP of the last decoded macroblock.
@param iCurrMbAddr Number of the current macroblock.
*/
int macroblock_I_partitionning(const PPS *Pps, RESIDU *picture_residu,  const unsigned char *ai_pcData,  int *aio_piPosition, 
							   SLICE *aio_pstSlice, DATA * aio_pstBlock, const VLC_TABLES * Vlc, 
							   unsigned char aio_tiNon_zero_count_cache [ ], unsigned char aio_pstSlice_table [ ], 
							   const short ai_iMb_x, const short ai_iMb_y, int *last_QP, int iCurrMbAddr)
{



	short intra4x4_pred_mode_cache[40];
	int mb_qp_delta;

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckIMbType(picture_residu -> MbType)){
		return 1;
	}
#endif


	//Updating the aio_pstSlice table in order to save in which slice each macroblock belong to
	picture_residu -> SliceNum  = aio_pstSlice_table [iCurrMbAddr] = aio_pstSlice -> slice_num ;

	if ( picture_residu -> MbType == INTRA_PCM )   {
		while ( !bytes_aligned(*aio_piPosition) ) {
			getNbits(ai_pcData, aio_piPosition, 1);//pcm_alignment_zero_bit = 
		}
		ParseIPCM(ai_pcData, aio_piPosition, picture_residu, aio_tiNon_zero_count_cache);
	} 
	else 	{
		//Updating the parameter in order to decode the VLC
		fill_caches_I( aio_pstSlice, picture_residu, 0, aio_tiNon_zero_count_cache, aio_pstBlock, aio_pstSlice_table, 
			intra4x4_pred_mode_cache, ai_iMb_x, ai_iMb_y, Pps -> constrained_intra_pred_flag);


		if ( Pps -> transform_8x8_mode_flag && picture_residu -> MbType == INTRA_4x4 && getNbits(ai_pcData, aio_piPosition, 1)){
			picture_residu -> Transform8x8 = picture_residu -> MbType = aio_pstBlock -> Transform8x8 = INTRA_8x8;
		}

		//Recovery of the prediction mode and the motion vectors for the macroblock 
		if(mb_pred_I(ai_pcData, aio_piPosition, picture_residu, picture_residu -> Intra16x16DCLevel, intra4x4_pred_mode_cache)){
			return 1;
		}


		if ( aio_pstBlock -> MbPartPredMode[0] != INTRA_16x16 ) {
			picture_residu -> Cbp = read_me(ai_pcData, aio_piPosition, aio_pstBlock -> MbPartPredMode[0]);
		}


		if ( picture_residu -> Cbp > 0 ||  (picture_residu -> MbType == INTRA_16x16)){
			mb_qp_delta = read_se(ai_pcData, aio_piPosition);

#ifdef TI_OPTIM
			*last_QP = picture_residu -> Qp = divide(*last_QP + mb_qp_delta + 52, 52) >> 8 ;
#else
			*last_QP = picture_residu -> Qp = (*last_QP + mb_qp_delta + 52) % 52;
#endif

			//Decoding process of the VLC 
			residual(ai_pcData, aio_piPosition, picture_residu, Vlc, aio_tiNon_zero_count_cache);
		} 
		else 
		{
			//In case of there is no error in the prediction
			init_mb_skip(aio_tiNon_zero_count_cache);
			picture_residu -> Qp = *last_QP;
		}
	}
	return 0;
}









/**
This function permits to recover the macroblock's data from the vlc. 
All the parameters decoded will be stored in differents structures or tables.

@param Pps PPS structure of the current video.
@param picture_residu Structure whixh contains information about the macroblock.
@param data The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param aio_pstSlice The aio_pstSlice structure.
@param block Contains all parameters of the current macroblock.
@param vlc The VLC tables in order to decode the Nal Unit.
@param non_zero_count_cache Specifies the coeff_token of each blocks 4x4 of a macroblock.
@param non_zero_count Specifies the coeff_token of each block of the picture.
@param aio_pstSlice_table Specifies in which aio_pstSlice belongs each macroblock of the picture.
@param intra_pred_mod Contains the prediction mode for each macroblock.
@param ai_iMb_x The x position of the macroblock in the picture.
@param ai_iMb_y The y position of the macroblock in the picture.
@param last_QP Give the QP of the last decoded macroblock.
@param iCurrMbAddr Number of the current macroblock.
@param mv_cache_l0 Table used to stock the motion vector of the current macroblock.
@param ref_cache_l0 Table used to stock the reference index of the current macroblock.
*/
int 	macroblock_P_partitionning (const PPS *Pps, RESIDU *Current_residu,  const unsigned char *ai_pcData,  int *aio_piPosition, 
									SLICE *aio_pstSlice, DATA * aio_pstBlock,const VLC_TABLES * Vlc, 
									unsigned char aio_tiNon_zero_count_cache [ ], unsigned char aio_pstSlice_table [ ], 
									const short ai_iMb_x, const short ai_iMb_y, int *last_QP, int iCurrMbAddr, 
									short mv_cache_l0[][2], short *ref_cache_l0)
{




	int dct_allowed = Pps -> transform_8x8_mode_flag;
	int mb_qp_delta;

#ifdef ERROR_DETECTION
	//Error detection
	if(ErrorsCheckPMbType(Current_residu -> MbType)){
		return 1;
	}
#endif

	//Current_residu the aio_pstSlice table in order to save in which slice each macroblock belong to
	Current_residu -> SliceNum = aio_pstSlice_table [iCurrMbAddr] = aio_pstSlice -> slice_num ;

	//Updating the parameter in order to decode the VLC
	fill_caches( aio_pstSlice, Current_residu, 0, aio_tiNon_zero_count_cache, aio_pstBlock, aio_pstSlice_table, ai_iMb_x, ai_iMb_y);


	//Recovery of the motion vectors for the sub_macroblock 
	if (aio_pstBlock -> NumMbPart == 4) {
		if(sub_mb_pred_P(&dct_allowed, ai_pcData, aio_piPosition, aio_pstSlice, Current_residu, mv_cache_l0, ref_cache_l0)){
			return 1;
		}
	}
	else{ 
		//Recovery of the prediction mode and the motion vectors for the macroblock 
		if(mb_pred_P(ai_pcData, aio_piPosition, aio_pstSlice, Current_residu, mv_cache_l0, ref_cache_l0)){
			return 1;
		}
	}

	Current_residu -> Cbp = read_me(ai_pcData, aio_piPosition, aio_pstBlock -> MbPartPredMode[0]);
	if ( dct_allowed && (Current_residu -> Cbp & 15)){
		if(getNbits(ai_pcData, aio_piPosition, 1))
			Current_residu -> Transform8x8 = aio_pstBlock -> Transform8x8 = INTRA_8x8;
	} 

	if ( Current_residu -> Cbp > 0){
		mb_qp_delta = read_se(ai_pcData, aio_piPosition);
#ifdef TI_OPTIM
		//Initialization of the macroblock neighbourhood
		*last_QP = Current_residu -> Qp = divide(*last_QP + mb_qp_delta + 52, 52) >> 8 ;
#else
		*last_QP = Current_residu -> Qp = (*last_QP + mb_qp_delta + 52) % 52;
#endif

		//Decoding process of the VLC 
		residual(ai_pcData, aio_piPosition, Current_residu, Vlc, aio_tiNon_zero_count_cache);
	} 
	else 
	{
		//In case of there is no error in the prediction
		init_mb_skip(aio_tiNon_zero_count_cache);
		Current_residu -> Qp = *last_QP;
	}

	return 0;
}
