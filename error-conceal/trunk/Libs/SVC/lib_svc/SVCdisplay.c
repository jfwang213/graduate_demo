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
#include "display_management.h"
#include "memory.h"


//SVC FIles
#include "symbol.h"
#include "svc_type.h"
#include "set_layer.h"
#include "SVCDisplay.h"
#include "ErrorDetection.h"
#include "assert.h"

#include "log.h"
#include "stdio.h"
#include "layer_utils.h"









/**
This function allows to display the right image according to it's poc.

@param wanted_id DqId to display
@param sps_id SPS structure of the current video.
@param Current_pic LIST_MMO structure with contains information on the current picture.
@param address_pic Address of the image to display in the decoded picture buffer.
@param x_size Width of the image to display.
@param y_size Height of the image to display.
@parma img_to_display Parameter which permits to know if there is an picture to display.
@param mmo MMO structure which contains all information about memory management of the video.
*/

void DisplayOrderSvc( decoder_context* pdecoder_context, DISPLAY_M* display_memory, int *x_size, 
					 int *y_size, int *Crop, int *img_to_display, MMO *Mmo)
{
	int j;
	//The picture management is conditioned if the current layer is allocated
	if (Mmo -> MemoryAllocation){
		//Just to avoid the AU parsing of players
		

		//Detect if we have waited enough decoded frame except for the first frame.
		if (0 == Mmo->num_free_poc_address){
				int BestPos;
				POC_PARAMS *Frame;
				int dsiplayBaseLayerId = get_layer_id(pdecoder_context->dq_to_display,pdecoder_context->layer_id_table);
				Frame = FindBestFrame(Mmo, Mmo -> poc_params, &BestPos, MIN_POC);

				if (!Frame -> is_ghost){
					//Try to know if we can release the memory address
					LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [dsiplayBaseLayerId];
					Mmo -> num_free_poc_address ++;
					CurrentDisplay(Mmo, LayerMmo, Frame -> Picture, display_memory);				
					logStr("display pic poc:%d\tmemory:%x\n",Frame -> Picture -> poc,Frame -> Picture->picture_memory_y);
					log_flush();
					printf("\t display :%d %d     %d %d\n", Mmo -> LayerMMO [0] . num_decoded_frame, Frame -> Picture -> poc, Frame -> x_size, Frame -> y_size);
					*img_to_display = 1;
					*x_size = Frame -> x_size;
					*y_size = Frame -> y_size;
					*Crop = Frame -> Crop;
					for (j = BestPos; j < Mmo -> nb_img_display - Mmo -> num_free_poc_address + 1; j++){
						memmove(&Mmo -> poc_params[j], &Mmo -> poc_params[j + 1],  1 * sizeof(POC_PARAMS));
					}
				}else{
					assert(0);
				}
		}else{
			//Not ready yet
			*img_to_display = 0;
		}
	}
}







