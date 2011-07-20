#include "bitstream.h"

int read_ue_C ( const unsigned char *tab,  int *pos )
{
	int i;
	int r_value;
	int buf = showNbits(tab, *pos, 24);
	int leadingzeroBits = 0;
	int test = 1 << 23;

	for ( i = 0; i < 24 && leadingzeroBits == 0; i++, test >>= 1)
		leadingzeroBits = buf & test;

	leadingzeroBits = i - 1;

	if(!leadingzeroBits){
		*pos += 1;
		return 0;
	}
	else if(leadingzeroBits < 12){
		*pos += (leadingzeroBits << 1) + 1;
		buf >>= (23 - (leadingzeroBits << 1));

		r_value = (buf - 1);

	}
	else	{
		// we calculate another time leadingzeroBits this time up to 32 bits
		int b = 0 ;
		int leadingzeroBits = 0 ;
		b = getNbits(tab, pos, 1);

		for (  ; !b ; leadingzeroBits++ ) {
			b = getNbits(tab, pos, 1);
		}

		r_value = leadingzeroBits ? (1 << leadingzeroBits) - 1 + getNbits(tab, pos, leadingzeroBits) : 0;
	}

	if(r_value < 0) {
		r_value = 0;
	}
	return r_value;
}