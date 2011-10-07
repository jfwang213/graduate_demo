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

#include "data.h"
#include "string.h"
#include "display_management.h"


/**
This function searches into the frame buffer a frame with a specify POC.
*/
int SearchSamePOC(POC_PARAMS *Frame, LIST_MMO *Current_pic, int LayerNumber){

	int i;
	int Pos = -1;

	for ( i = 0; i < NUM_POC_PARAMS; i++){
		POC_PARAMS *CurrFrame = &Frame[NUM_POC_PARAMS - 1 - i];
		if (CurrFrame -> Picture -> poc == Current_pic -> poc){
			//A picture of a different layer exits
			//We just remember its position
			if (CurrFrame -> LayerId < LayerNumber){
				Pos = NUM_POC_PARAMS - i - 1;
			}
			if (CurrFrame -> Picture->picture_memory_y == Current_pic -> picture_memory_y){
				//SNR picture, same memory adresss. do nothing.
				CurrFrame -> Picture -> PicToDisplay = Current_pic -> PicToDisplay;
				CurrFrame -> LayerId = LayerNumber;
				Pos = -2;
			}
			break;
		}
	}

	return Pos;
}


/**
This function stores into the frame buffer the input frame.
*/
void StorePicture(POC_PARAMS *Frame, LIST_MMO *Current_pic, SPS *sps, int layer_number){

	//Stock the new frame in reorder buffer
	memcpy(Frame -> Picture , Current_pic, sizeof(LIST_MMO));

	Frame -> x_size = ((sps -> pic_width_in_mbs) << 4) - sps -> CropRight - sps -> CropLeft;
	Frame -> y_size = ((sps -> pic_height_in_map_units) << 4) - sps -> CropBottom - sps -> CropTop;
	Frame -> Crop = (sps -> CropTop << 24) + (sps -> CropBottom << 16) + (sps -> CropLeft << 8) + sps -> CropRight;
	Frame -> LayerId = layer_number;


	//For SVC in case of ghost picture
	if (Current_pic -> displayed == 255){
		Frame -> is_ghost = 1;
	} else {
		Frame -> is_ghost = 0;
	}
}

POC_PARAMS* FindSpecificFrame(MMO *mmo,int poc,char bBase)
{
	POC_PARAMS* result = NULL;
	POC_PARAMS* frames = mmo->poc_params;
	int i;
	for (i = 0;i<mmo->nb_img_display-mmo->num_free_poc_address;i++)
	{
		if (frames[i].Picture->poc == poc)
		{
			if (bBase == frames[i].Picture->bBasePic)
			{
				result = frames+i;
				break;
			}
			else if (NULL == result)
			{
				result = frames+i;
			}
		}
	}
	return result;
}

/**
Finds the next picture to display.
*/
POC_PARAMS* FindBestFrame(MMO *mmo, POC_PARAMS *FrameList, int *BestPos, int MinPoc){

	int j, IDR = -1;
	int best_poc = MAX_POC;
	*BestPos = -1;

	//Find IDR frame with POC == 0
	for(j = 0; j < mmo -> nb_img_display - mmo -> num_free_poc_address ; j++){
		if (FrameList[j] . Picture -> poc == 0){	
			if(j != 0){
				IDR = j;
				break;
			}
		}
	}
			
		if(IDR != -1){
		//Just take the minus one until the next IDR
		for(j = 0; j < IDR ; j++){
			if ( FrameList[j] . Picture -> poc < best_poc && FrameList[j] . Picture -> poc > MinPoc) {
				best_poc = FrameList[j] . Picture -> poc;
				*BestPos = j;
			}
		}
	}else {
		for(j = 0; j < mmo -> nb_img_display - mmo -> num_free_poc_address ; j++){
			//Test if exists a frame with a POC inferior to the first in the head.
			//This frame must have a POC superior to the previous one displayed
			//Or in case of multiple IDR with the JSVM, POC can be negative
			if ( FrameList[j] . Picture -> poc < best_poc && FrameList[j] . Picture -> poc > MinPoc){
				best_poc = FrameList[j] . Picture -> poc;
				*BestPos = j;
			}
		}
	}

	if(*BestPos == -1){
		//Best poc not found
		//Just take the minus one
		for(j = 0; j < mmo -> nb_img_display - mmo -> num_free_poc_address ; j++){
			if ( FrameList[j] . Picture -> poc < best_poc) {
				best_poc = FrameList[j] . Picture -> poc;
				*BestPos = j;
			}
		}
	}

	return &FrameList [*BestPos];
}







/**
Determines if the memory of frame to display has to be released.
*/
void CurrentDisplay(MMO *CurrMmo, LAYER_MMO *LayerMmo, LIST_MMO *Frame, DISPLAY_M* display_memory){

	int j;
	int keep = 0;
	DEP_MMO* dep_mmo = LayerMmo->dep_mmo;
	const int best_poc = Frame -> poc;
	display_memory->memory_size = Frame->memory_size;
	display_memory->display_y = Frame->picture_memory_y;
	display_memory->display_u = Frame->picture_memory_u;
	display_memory->display_v = Frame->picture_memory_v;
	for( j = 0; j < dep_mmo -> ShortRefCount; j++){
		if ((best_poc == dep_mmo -> ShortRef[j] . poc) &&
			(dep_mmo -> ShortRef[j] . reference == 3) &&
			(dep_mmo -> ShortRef[j].picture_memory_y == display_memory->display_y)){
			keep = 1;
			dep_mmo -> ShortRef[j] . displayed = 1;
			break;
		}
	}

	for( j = 0; j < MAX_REF && !keep; j++){
		if (	(best_poc == dep_mmo -> LongRef[j] . poc) && 
				(dep_mmo -> LongRef[j] . reference == 3) && 
				(dep_mmo -> LongRef[j] . picture_memory_y == display_memory->display_y) ){
			keep = 1;
			dep_mmo -> LongRef[j] . displayed = 1;
			break;
		}
	}
	for( j = 0; j < MAX_REF && !keep; j++){
		if (	(best_poc == dep_mmo -> LongRefBase[j] . poc) && 
			(dep_mmo -> LongRef[j] . reference == 3) && 
			(dep_mmo -> LongRef[j] . picture_memory_y == display_memory->display_y) ){
				keep = 1;
				dep_mmo -> LongRef[j] . displayed = 1;
				break;
		}
	}

	display_memory->to_release = !keep;
}
