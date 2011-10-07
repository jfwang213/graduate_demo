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








//AVC Files
#include "type.h"

//SVC Files
#include "set_layer.h"
#include "LayerManagement.h"



static int ARG_LAYER = 0;
static int SAVED_DQID = 0;
static int LAYER_CHANGED = 0;
static int DEC_MAX_LAYER = 0;
static int LAST_DQID = -1;
static int SEQ_TEMP_ID = 255;
static int MAX_TEMP_ID = 255;

/* 

Initilaization part
*/

/*
Allow to initialize all parameters at the beginning of the decoding process
*/
void initialize_layer(int num_of_layers){
	 ARG_LAYER = num_of_layers & 0xff;
	 SetSeqTempId(num_of_layers >> 16);
}

void init_max_layer(int num_of_layer){

	DEC_MAX_LAYER = num_of_layer - 1;
}




/*
Functions with allow to store or to get parameters value.
*/
void ResetLayerChanged(){
	LAYER_CHANGED = 0;
}



//Get
int GetArgLayer (){
	return ARG_LAYER ;
}

int GetLayerChanged(){
	return LAYER_CHANGED;
}

int GetSaveDqId(){
	return SAVED_DQID;
}

int GetLastDqId(){
	return LAST_DQID;
}

int GetMaxDecLayer(){
	return DEC_MAX_LAYER;
}



int GetSeqTempId(){
	if(SEQ_TEMP_ID > MAX_TEMP_ID){
		SEQ_TEMP_ID = MAX_TEMP_ID;
	}
	return SEQ_TEMP_ID;
}



//Set
void SetLayerChanged(){
	LAYER_CHANGED = 1;
}


void SetSeqTempId(int TempId){
	if(MAX_TEMP_ID >= TempId){
		SEQ_TEMP_ID = TempId;
	}
}

void SetLastDqId( int DqID){
	LAST_DQID = DqID;
}

void SetArgLayer ( int DqId){
	ARG_LAYER = DqId;
}

void SetSaveDqId ( int DqId){
	SAVED_DQID = DqId;
}


void SetSeqMaxTempId(int TempId){
	MAX_TEMP_ID = TempId;
}
