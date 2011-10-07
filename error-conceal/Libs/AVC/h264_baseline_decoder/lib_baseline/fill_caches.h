/*****************************************************************************
*
*  Open SVC Decoder developped in IETR image lab
*
*
*
*              M�d�ric BLESTEL <mblestel@insa-rennes.Fr>
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




#ifndef FILL_CACHES_H
#define FILL_CACHES_H

#include "type.h"
void FillNonZeroCount( const RESIDU CurrResidu[], const DATA *CurrBlock, const int MbStride,const int top_type, 
					  const int  left_type , unsigned char *NonZeroCountCache);

void fill_caches (const SLICE *aio_pstSlice, const RESIDU *Residu, const int ai_iEntropy_coding_mode_flag, 
				  unsigned char aio_tiNon_zero_count_cache [ ], const DATA *CurrBlock, const unsigned char aio_pstSlice_table [ ], 
				  const short ai_iMb_x, const short ai_iMb_y);


void fill_caches_I (const SLICE *aio_pstSlice, const RESIDU *Residu, const int ai_iEntropy_coding_mode_flag, 
					unsigned char aio_tiNon_zero_count_cache [ ], const DATA *CurrBlock, 
					const unsigned char aio_pstSlice_table [ ], short aio_tiIntra4x4_pred_mode_cache[], 
					const short ai_iMb_x, const short ai_iMb_y, const int ai_iConstrained_intra_pred);


void fill_caches_motion_vector(  const SLICE *aio_pstSlice, const int ai_iB_stride, const int ai_iB8_stride, short aio_tiRef_cache_l0 [  ], 
							   short aio_tiMv_cache_l0[  ][2], const unsigned char aio_pstSlice_table [ ], const RESIDU *Residu, 
							   short ai_tiMv[  ], const short ai_tiRef[ ],  const int ai_iX, const int ai_iY);


/**
This function permits to update the neighbourhood of the current macroblock in case of a skipped macroblock.

@param aio_tiNon_zero_count_cache A cache table where coeff_token is stocked for each 4x4 block of the current macroblock.
*/
static __inline void init_mb_skip ( unsigned char aio_tiNon_zero_count_cache [] )
{

	aio_tiNon_zero_count_cache [4 + 8 * 1] = aio_tiNon_zero_count_cache [5 + 8 * 1] = aio_tiNon_zero_count_cache [6 + 8 * 1] = 
	aio_tiNon_zero_count_cache [7 + 8 * 1] = aio_tiNon_zero_count_cache [4 + 8 * 2] = aio_tiNon_zero_count_cache [5 + 8 * 2] = 
	aio_tiNon_zero_count_cache [6 + 8 * 2] = aio_tiNon_zero_count_cache [7 + 8 * 2] = aio_tiNon_zero_count_cache [4 + 8 * 3] = 
	aio_tiNon_zero_count_cache [5 + 8 * 3] = aio_tiNon_zero_count_cache [6 + 8 * 3] = aio_tiNon_zero_count_cache [7 + 8 * 3] = 
	aio_tiNon_zero_count_cache [4 + 8 * 4] = aio_tiNon_zero_count_cache [5 + 8 * 4] = aio_tiNon_zero_count_cache [6 + 8 * 4] = 
	aio_tiNon_zero_count_cache [7 + 8 * 4] = 0;

	aio_tiNon_zero_count_cache [1 + 8 * 1] = aio_tiNon_zero_count_cache [2 + 8 * 1] = aio_tiNon_zero_count_cache [1 + 8 * 2] = 
	aio_tiNon_zero_count_cache [2 + 8 * 2] = aio_tiNon_zero_count_cache [1 + 8 * 4] = aio_tiNon_zero_count_cache [2 + 8 * 4] =
	aio_tiNon_zero_count_cache [1 + 8 * 5] = aio_tiNon_zero_count_cache [2 + 8 * 5] = 0;
}



/**
This function initialises the tab aio_tiNon_zero_count_cache when the macroblock is coded in I_PCM way.

@param non_zero_count_cache Specifies the coeff_token of each blocks 4x4 of a macroblock.
*/
static __inline void init_mb_IPCM ( unsigned char aio_tiNon_zero_count_cache [ ] )
{	
	aio_tiNon_zero_count_cache [4 + 8] = aio_tiNon_zero_count_cache [5 + 8] = aio_tiNon_zero_count_cache [6 + 8] = 
	aio_tiNon_zero_count_cache [7 + 8] = aio_tiNon_zero_count_cache [4 + 16] = aio_tiNon_zero_count_cache [5 + 16] = 
	aio_tiNon_zero_count_cache [6 + 16] = aio_tiNon_zero_count_cache [7 + 16] = aio_tiNon_zero_count_cache [4 + 24] = 
	aio_tiNon_zero_count_cache [5 + 24] = aio_tiNon_zero_count_cache [6 + 24] = aio_tiNon_zero_count_cache [7 + 24] = 
	aio_tiNon_zero_count_cache [4 + 32] = aio_tiNon_zero_count_cache [5 + 32] = aio_tiNon_zero_count_cache [6 + 32] = 
	aio_tiNon_zero_count_cache [7 + 32] = aio_tiNon_zero_count_cache [1 + 8] = aio_tiNon_zero_count_cache [2 + 8] = 
	aio_tiNon_zero_count_cache [1 + 16] = aio_tiNon_zero_count_cache [2 + 16] = aio_tiNon_zero_count_cache [1 + 32] = 
	aio_tiNon_zero_count_cache [2 + 32] = aio_tiNon_zero_count_cache [1 + 40] = aio_tiNon_zero_count_cache [2 + 40] = 16;
}


static __inline void FillMvCacheTopPosition(short MvCache[][2], short *MvdL0){
	MvCache [4][0] = *(MvdL0++);
	MvCache [4][1] = *(MvdL0++);
	MvCache [5][0] = *(MvdL0++);
	MvCache [5][1] = *(MvdL0++);
	MvCache [6][0] = *(MvdL0++);
	MvCache [6][1] = *(MvdL0++);
	MvCache [7][0] = *(MvdL0++);
	MvCache [7][1] = *(MvdL0++);
}


static __inline void FillMvCacheLeftPosition(short MvCache[][2], short *MvdL0, int Stride){

	MvCache[11][0] = *(MvdL0++);
	MvCache[11][1] = *MvdL0;

	MvdL0 += Stride - 1;
	MvCache[19][0] = *(MvdL0++);
	MvCache[19][1] = *MvdL0;

	MvdL0 += Stride - 1;
	MvCache [27][0] = *(MvdL0++);
	MvCache [27][1] = *MvdL0;

	MvdL0 += Stride - 1;
	MvCache [35][0] = *(MvdL0++);
	MvCache [35][1] = *MvdL0;
}

static __inline void FillRefCacheLeftPosition(short *RefCache, short FirstBlock, short ThirdBlock){
	RefCache[11] = RefCache[19] = FirstBlock;
    RefCache[27] = RefCache[35] = ThirdBlock;
}

static __inline void FillRefCacheTopPosition(short *RefCache, short FirstBlock, short SecondBlock){
	RefCache[4] = RefCache[5] = FirstBlock;
    RefCache[6] = RefCache[7] = SecondBlock;
}

#endif
