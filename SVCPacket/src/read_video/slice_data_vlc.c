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


#include "slice_data_vlc.h"
#include "bitstream.h"





/**
This function permits to detect the end of the Nal.

@param ai_pcData The NAL unit.
@param aio_piPosition The current aio_piPosition in the NAL.
@param ai_piNalBytesInNalunit The sizes of the NAL unit in bytes.

*/
int more_rbsp_data ( const unsigned char *ai_pcData, const int *aio_piPosition, const int ai_piNalBytesInNalunit)
{

	
	int byteoffset = *aio_piPosition >> 3 ;


	if ( byteoffset  < ai_piNalBytesInNalunit - 1 ) {
		return 1 ;
	}else {
		int  cnt = 0 ;
		int localPosition = *aio_piPosition;	
		int bitoffset = 6 - (localPosition & 7);

		// read one bit
		int ctr_bit = showNbits(ai_pcData, localPosition, 1);

		// a stop bit has to be one
		if ( !ctr_bit ) return 1 ;

		localPosition++;

		while (bitoffset >= 0)	{
			ctr_bit = showNbits(ai_pcData, localPosition, 1);   // set up control bit

			if (ctr_bit) cnt++;

			bitoffset--;
			localPosition++;
		}

		return (0 != cnt);
	}
}

