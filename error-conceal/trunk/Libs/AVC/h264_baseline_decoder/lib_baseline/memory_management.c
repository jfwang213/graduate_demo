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



#include <stdlib.h>
#ifndef POCKET_PC
#include <string.h>
#include <stdio.h>

#endif


#ifdef WIN32
#include "windows.h"
#endif

#include "clip.h"
#include "symbol.h"
#include "memory_management.h"
#include "picture_management.h"





/**
This function allows to copy n element from a buffer to an other one with optimizations.

@param ao_pucDest Destination of the copy.
@param ai_pucOrig Source of the copy.
@param ai_iSize Number of element to copy.
*/
void optim_memcpy(	unsigned char* RESTRICT ao_pucDest, const unsigned char* RESTRICT ai_pucOrig, const int ai_iSize)
{
	int i;
	int cnt8 = ai_iSize&(~7);

	for(i = 0; i < cnt8; i++) {
		*(ao_pucDest++) = *(ai_pucOrig++);
	}

	for(i = 0; i < (ai_iSize&7); i++)	{
		*(ao_pucDest++) = *(ai_pucOrig++);
	}
}




/**
This function allows to copy n element from a buffer to an other one with optimizations and with 64 bits aligned.

@param ao_pucDest Destination of the copy.
@param ai_pucOrig Source of the copy.
@param ai_iSize Number of element to copy.
*/
void optim_memcpy_aligned64(unsigned char* RESTRICT ao_pucDest, const unsigned char* RESTRICT ai_pucOrig, const int ai_iSize)
{
	int i;
	int cnt16 = (ai_iSize)&(~15);

#ifdef TI_OPTIM
	_nassert(((unsigned int)ao_pucDest)%8==0);
	_nassert(((unsigned int)ai_pucOrig)%8==0);
	_nassert((cnt16)%16==0);
#pragma MUST_ITERATE(  ,  , 16 );
#endif

	for(i = 0; i < cnt16; i++)
	{
		*(ao_pucDest++) = *(ai_pucOrig++);
	}


	for(i = 0; i < (ai_iSize&15); i++)
	{
		*(ao_pucDest++) = *(ai_pucOrig++);
	}
}





/**
This function allows to initialize the decoded picture buffer address according to the size of the picture.

@param interval Size in pixel of a padded picture.
@param Dpb_depth Depth of the decoded picture buffer.
@param Dpb_display Depth of the display buffer.
@param nb_of_layers Number of layer choosen during the code generation.
@param num_layer Number of layer choosen to be displayed.
@param mmo MMO structure which contains all information about memory management of the video.
*/

void set_decoding_dpb(int interval, int Dpb_depth, int Dpb_display, int nb_of_layers, MMO *mmo){


	int NumOfPic = MIN(Dpb_depth + Dpb_display + nb_of_layers - 1, 32);


}



/**
This function allows to initialize all the decoded picture buffer address according to the size of the picture.

@param num_layer Number of layer choosen to be displayed.
@param interval Size in pixel of a padded picture.
@param mv_interval Size in pixel of all the motion vector of one picture.
@param Dpb_depth Depth of the decoded picture buffer.
@param nb_of_layers Number of layer choosen during the code generation.
@param Dpb_display Depth of the display buffer.
@param mmo MMO structure which contains all information about memory management of the video.
*/
void initialize_address(int total_memory, const int mv_memory, int nb_of_layers, int interval, int mv_interval, SPS *sps_id, MMO *mmo){



}





void avc_calculate_dpb(const int total_memory, const int mv_memory, MMO *mmo_struct, SPS *sps){


	
}





/**
This function allows to initialize all the decoded picture buffer address according to the size of the picture.


@param Dpb_depth Depth of the decoded picture buffer.
@param Nb_Layer Number of layer choosen during the code generation.
@param mmo MMO structure which contains all information about memory management of the video.
@param ai_pstRefPicListL0 Refernce list l0.
@param ai_pstRefPicListL1 Refernce list l1.
*/
void idr_flush_dpb(int Nb_Layer, MMO *mmo, LIST_MMO *RefPicListL0, LIST_MMO *RefPicListL1){

	int i,j;


	for(i = 0; i < 16; i++){
		RefPicListL0[i] . pic_num = -1;
		RefPicListL0[i] . poc = MIN_POC;
		RefPicListL0[i] . ref_count_l0 = -1;
		RefPicListL0[i] . reference = -1;
		RefPicListL1[i] . pic_num = -1;
		RefPicListL1[i] . poc = MIN_POC;
		RefPicListL1[i] . ref_count_l0 = -1;
		RefPicListL1[i] . reference = -1;
	}

	for(j = 0; j < Nb_Layer; j++){
		LAYER_MMO *LayerMmo = &mmo -> LayerMMO [j];
		DEP_MMO* dep_mmo = LayerMmo->dep_mmo;
		remove_all(mmo, dep_mmo);
		dep_mmo -> RefCountL0 = 0;
		dep_mmo -> prev_frame_num = 0;
		dep_mmo -> prev_frame_num_offset = 0;
	}
}



