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



#ifndef SVC_TYPE_H
#define SVC_TYPE_H
#include "type.h"
#include "list_head.h"

//mbl reduce memory

typedef struct {
	
	int DqIdMax;
	int SetLayer;
	int TemporalId;
	int NextDqId; // Or Max
	char layer_id_table[8];
	char UsedSpatial[8][8];

} ConfigSVC;

typedef struct {
	
	short RefIdc;

	//Must be in int
	int PartIdc;
	int MvIndex;
	int RefIndex;
} REFPART;




typedef void    (*Sub_P_svc)(short mv_cache [][2], short ref_cache [], const DATA * Block,  int mbPartIdx, short mv_l0[], short *ref_bas_l0, const NAL *Nal, REFPART RefPartition[16]);

typedef void    (*P_svc)( short mv_cache [][2], short ref_cache [], const RESIDU *CurrResidu, const DATA *Block, const Sub_P_svc decode_sub[]
			, unsigned char image [], unsigned char image_Cb [], unsigned char image_Cr []
			, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
			, const NAL *Nal, LIST_MMO *RefPicListL0, short mv_l0[], short *ref_bas_l0, const STRUCT_PF *vectors, REFPART RefPartition[16]);


typedef void    (*Sub_B_svc)( short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short aio_tref_cache_l0 [],short aio_tref_cache_l1 []	, const DATA *aio_pBlock
					, int mbPartIdx, int SubMbType, short *mv_bas_l0, short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1, const NAL *Nal, REFPART RefPartition[16]) ;

typedef void    (*B_svc)( short aio_tmv_cache_l0 [][2], short aio_tmv_cache_l1 [][2], short aio_tref_cache_l0 [], short aio_tref_cache_l1 []
						, const RESIDU *CurrResidu, const DATA *aio_pBlock, Sub_B_svc aio_tdecode_sub[]
						, unsigned char image[], unsigned char image_Cb[], unsigned char image_Cr[]
						, const short x, const short y, const short PicWidthInPix, const short PicHeightInPix
						, const NAL *Nal, LIST_MMO *   RefPicListL0, LIST_MMO *  RefPicListL1
						, short *mv_bas_l0, short *mv_bas_l1, short *ref_bas_l0, short *ref_bas_l1, const STRUCT_PF *vectors
						, REFPART RefPartition[16]) ;


 typedef struct {

	B_svc	  B_vectors[5];
	Sub_B_svc B_sub_vectors[13];
	P_svc	  P_vectors[5];
	Sub_P_svc P_sub_vectors[4];
 }SVC_VECTORS;


#endif
