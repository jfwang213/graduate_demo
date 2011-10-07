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



#ifndef TI_OPTIM
#ifndef POCKET_PC




#include "stdio.h"
#include "string.h"
#include "stdlib.h"


//SVC Files
#include "LayerManagement.h"

#ifdef HAVE_SDL
#ifdef WIN32
#include <conio.h>


int KeyboardEvent(){


	if(_kbhit()){
		return 1;
	}

	return 0;
}
#else //Not win32
#include <SDL/SDL_events.h>



int KeyboardEvent(){


	SDL_Event event; /* Event structure */

	/* Check for events */
	while(SDL_PollEvent(&event)){  /* Loop until there are no events left on the queue */
		switch(event.type){  /* Process the appropiate event type */
		   case SDL_KEYDOWN:  /* Handle a KEYDOWN event */
			   return 1;
		}
	}
	return 0;
}
#endif //WIN32






//pc

//Can't set directly LAYER_CHANGED to 1, due to dsp decoding.
//This function stay on PC.
void choose_layer (ConfigSVC *Config)
{
	if ( KeyboardEvent() ) {
		printf("Enter the number of the layer to decode\n");
		scanf("%d", &Config -> SetLayer);
		printf("Enter the maximal temporal_id to decode\n");
		scanf("%d", &Config -> TemporalId);
	}
}


#endif //HAVE_SDL



//pc
void get_layer(int argc, char **argv, decoder_context *pdecoder_context)
{
	int i;
	int found = 0;
	pdecoder_context -> select_layer[0]= 0;
	pdecoder_context -> temporal_id[0] = 255;

	for ( i = 1 ; i < argc ; i++ ) 	{
		if (strcmp("-layer", argv[i]) == 0 && i < argc - 1 ) {
			i++;
			pdecoder_context -> select_layer[0] = atoi(argv[i]);
			found = 1;
		}
		if (strcmp("-tempId", argv[i]) == 0 && i < argc - 1 ) {
			i++;
			pdecoder_context -> temporal_id[0] = atoi(argv[i]);
			found = 1;
		}
	}

	if ( !found ) 	{
		printf("Cannot open determine the layer to display %d \n", pdecoder_context -> select_layer[0]);
		printf("Help -layer layer_num\n");
		printf("Layer Forced to 0\n");
	}else{
		SetSaveDqId (pdecoder_context -> select_layer[0]);
		SetArgLayer (pdecoder_context -> select_layer[0]);
		pdecoder_context->dq_to_display = (pdecoder_context->select_layer[0])&255;
		SetSeqMaxTempId(pdecoder_context->temporal_id[0]);
	}
}
#endif //POCKET_PC

#else //TI_OPTIM

#include "stdio.h"
#include "string.h"
#include "stdlib.h"


//SVC Files
#include "LayerManagement.h"

void choose_layer (ConfigSVC *Config)
{

	if (GetLastDqId() == -1){
		int DecMaxLayer = GetMaxDecLayer();
		int WantedDqId = Config -> SetLayer;
		if ( WantedDqId >= 15 && WantedDqId <= (DecMaxLayer << 4) || WantedDqId < 16 && WantedDqId <= DecMaxLayer ){
			SetLastDqId(WantedDqId);
		}else {
			WantedDqId = 0;
			SetLastDqId (0);
		}
	}

/*	if ( KeyboardEvent() )    {
		int Temporal = 0;
		printf("Enter the number of the layer to decode\n");
		scanf("%d", &Config -> SetLayer);
		printf("Enter the maximal temporal_id to decode\n");
		scanf("%d", &Config -> TemporalId);
	}*/
}






//pc
void get_layer(int argc, char **argv, ConfigSVC *Config)
{
	int i;
	int found = 0;
	Config -> SetLayer = 0;
	Config -> TemporalId = 255;

	for ( i = 1 ; i < argc ; i++ ) 	{
		if (strcmp("-layer", argv[i]) == 0 && i < argc - 1 ) {
			i++;
			Config -> SetLayer = atoi(argv[i]);
			found = 1;
		}
		if (strcmp("-tempId", argv[i]) == 0 && i < argc - 1 ) {
			i++;
			Config -> TemporalId = atoi(argv[i]);
			found = 1;
		}
	}

	if ( !found ) 	{
		printf("Cannot open determine the layer to display %d \n", Config -> SetLayer);
		printf("Help -layer layer_num\n");
		printf("Layer Forced to 0\n");
	}else{
		SetSaveDqId (Config -> SetLayer);
		SetArgLayer (Config -> SetLayer);
	}
}

#endif TI_OPTIM
