/*****************************************************************************
*
*  SVC4DSP developped in IETR image lab
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


#include "SVCDecoder_ietr_api.h"


//AVC Files
#include "nal.h"
#include "sps.h"
#include "Read_h264.h"
#include "pic_list_svc.h"

//SVC Files
#include "svc_type.h"
#include "ParseAU.h"
#include "LayerManagement.h"
#include "SliceHeaderExtension.h"
#include "nal_unit_header_svc_extension.h"





int Geth264NalSize(const unsigned char *buf, int buf_size, int *bufindex){

	int nalsize = 0;
	int buf_index = *bufindex;

	// start code prefix search
	for(; buf_index + 3 < buf_size; buf_index++) {
		// This should always succeed in the first iteration.
		if(buf[buf_index] == 0 && buf[buf_index + 1] == 0 && buf[buf_index + 2] == 1)
		break;
	}

	if(buf_index + 3 >= buf_size) {
		return -1;
	}

	*bufindex = buf_index += 3;
	nalsize = buf_index;

	for(; buf_index < buf_size; buf_index++) {
		// This should always succeed in the first iteration.
		if(buf[buf_index] == 0 && buf[buf_index + 1] == 0 && buf[buf_index + 2] == 1){
		break;
		}
	}

	if( buf_index == buf_size){
		//To emulate the end of a start code
		buf_index += 1;
	}

	//Minus one just to be on the right number. To check.
	nalsize = buf_index - nalsize - 1;
	return nalsize;
}



int GetDqIdMax(const unsigned char *buf, int buf_size, int nal_length_size, int *DqidTable, int is_avc){

	int bufindex = 0;
	int DqIdMax = 0;
	int index = 1;
	DqidTable[0] = 0;

	for(;;)
	{
		int i, nalsize = 0;
		if(bufindex >= buf_size) 
			break;

		if (is_avc){
			for(i = 0; i < nal_length_size; i++){
				nalsize = (nalsize << 8) | buf[bufindex++];
			}
		}else {
			nalsize = Geth264NalSize(buf, buf_size, &bufindex);
		}


		if (((buf[bufindex] & 0x1f) == 14) || ((buf[bufindex] & 0x1f) == 20)){
			//Load the 4 first bytes
			int Load = buf[bufindex + 2];
			int Dependency = Load & 0x70;
			int Quality = Load & 0x0f;
			int DqId = Dependency + Quality;
			if (DqIdMax < DqId){
				DqIdMax = DqId;
				//Problem with mplayer.
				//Nalunittype == 14 are flushed
				if(((buf[bufindex] & 0x1f) == 20)){
					DqidTable [index] = DqId;
					index ++;
				}
			}
		}		
		bufindex += nalsize;
	}
	
	return DqIdMax;
}




/**
This function decode the slice header of all enhancement to determine the configuration of the stream.
*/

void ParseAuPlayers(void *PlayerStruct, const unsigned char *buf, int buf_size, int nal_length_size, int is_avc){

	int buf_index = 0;
	OPENSVCFRAME Picture[1];

	for(;;)
	{
        int i, nalsize = 0;
        if(buf_index >= buf_size) 
			break;

		if (is_avc){
			for(i = 0; i < nal_length_size; i++){
				nalsize = (nalsize << 8) | buf[buf_index++];
			}
		}else {
			nalsize = Geth264NalSize(buf, buf_size, &buf_index);
		}
			        
		//DqIdmax == 255 in order not ot display frame
 		decodeNAL(PlayerStruct, buf + buf_index, nalsize, Picture, 255);
        buf_index += nalsize;
    }
}
