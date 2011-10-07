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



#define WRITE_YUV_
/*

#include "string.h"

//AVC Files
#include "type.h"
#include "data.h"
#include "display_management.h"


//SVC Files
#include "svc_type.h"
#include "set_layer.h"
#include "SVCDisplay.h"*/

//Interface
#include "SVCDecoder_ietr_api.h"


static int done = 0;



/**
This function allow to initialize the image pointeur when the main architecture is only tcpmp
*/
void Extract_tcpmp(int xsize, int ysize, int edge, int Crop, unsigned char img_luma_in[], unsigned char img_Cb_in[],
				   unsigned char img_Cr_in[], int address_pic, OPENSVCFRAME *CurrFrame){

	int Right = Crop & 0xff;
	/*int Left  = (Crop >> 8) & 0xff;
	int Bottom  = (Crop >> 16) & 0xff;
	int Top  = (Crop >> 24) & 0xff;*/
	int Xsize = xsize + Right;

	const int offset_L = ((Xsize << 4) + edge + address_pic);
	const int offset_C = ((Xsize << 2) + edge/2 + (address_pic >> 2));

	*CurrFrame -> pY = &img_luma_in[offset_L];
	*CurrFrame -> pU = &img_Cb_in[offset_C];
	*CurrFrame -> pV = &img_Cr_in[offset_C];

	CurrFrame -> Width = xsize - 32 + Right;
	CurrFrame -> Height = ysize - 32;	
	done = 1;
#ifdef WRITE_YUV
	WriteOutputVideo(xsize, ysize, *CurrFrame -> pY, *CurrFrame -> pU, *CurrFrame -> pV);
#endif
}


/**
This function serves only when the core of the API is done on DSP, in order to initalize the image pointer
*/
void Display_tcpmp(int xsize, int ysize, int edge, unsigned char img_luma_in[], unsigned char img_Cb_in[], 
				   unsigned char img_Cr_in[], OPENSVCFRAME *CurrFrame){


	if(!done){
		*CurrFrame -> pY = &img_luma_in[0];
		*CurrFrame -> pU = &img_Cb_in[0];
		*CurrFrame -> pV = &img_Cr_in[0];
		CurrFrame -> Width = xsize;
		CurrFrame -> Height = ysize;	
	}
}
