/*****************************************************************************
   *
   *  SVC4DSP developped in IETR image lab
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


//SVC Files
#include "set_layer.h"
#include "ControlLayer.h"
#include "LayerManagement.h"

static int CMD_TEMPORAL_ID = -1;

void getLayer(int *num_layer){

	*num_layer = GetArgLayer();
}


// mplayer with cmd command
int StartOnTemporalId(int num_layer)
{
	//Only one call allowed
	if (CMD_TEMPORAL_ID == -1){
		CMD_TEMPORAL_ID = num_layer;
	}
	return 0;
}







// player
int setLayer(int num_layer)
{
	int LastDqId = GetLastDqId();
	if(num_layer != LastDqId){
		SetSaveDqId (num_layer);
		SetLayerChanged();
	}

	return 0;
}




void SetSVCTemporalIdUp(){
	int TemporalId = GetSeqTempId();
	SetSeqTempId(TemporalId + 1);
}

void SetSVCTemporalIdDown(){
	int TemporalId = GetSeqTempId();
	if(TemporalId > 0){
		SetSeqTempId(TemporalId - 1);
	}
}





/**
Used by TCPMP in order to change of layer.
*/
void PlayerSetLayer(int DqId, int *CurrDqId){

	setLayer(DqId);
	*CurrDqId = DqId;
}



/**
Used by TCPMP in order to change of layer.
*/
void UpdateLayer(int *DqIdTable, int *CurrDqId, int MaxDqId, int Command){

	int i;

	//Update Temporal id
	if (CMD_TEMPORAL_ID != -1){
		//only once
		SetSeqTempId(CMD_TEMPORAL_ID);
		CMD_TEMPORAL_ID = -1;
	}

	//Get current layer and max dqDid
	for( i = 0; i < 8 && DqIdTable [i] != -1; i++){
		if(*CurrDqId == DqIdTable [i]){
			break;
		}
	}

	switch(Command){
		case 0://Set layer Down
			if(i > 0){
				PlayerSetLayer(DqIdTable [i - 1], CurrDqId);
			}
			break;
		case 1://Set layer up
			if(i < 7 && DqIdTable [i + 1] != -1){
				PlayerSetLayer(DqIdTable [i + 1], CurrDqId);
			}
			break;
		case 2:
			//Set layer max
			PlayerSetLayer(MaxDqId, CurrDqId);
			break;	
	}
}
