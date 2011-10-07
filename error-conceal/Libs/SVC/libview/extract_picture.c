/*****************************************************************************
   *
   *  H264DSP developped in IETR image lab
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




#ifdef MMX
#ifdef POCKET_PC
#include <mmintrin.h>
#include <cmnintrin.h> 
#else
#include <emmintrin.h>
#include <mmintrin.h>
#endif
#endif



/**
This function allows to remove the padding of the image.

@param xsize Width of the picture.
@param ysize Heigth of the picture.
@param egde Edge of the picture.
@param img_luma_in Decoded picture Buffer.
@param img_Cb_in Decoded picture Buffer Cb.
@param img_Cr_in Decoded picture Buffer Cr.
@param address_pic Position of the current frame in the dpb.
@param img_luma_out picture unpadded.
@param img_Cr_out Chroma Cb unpadded.
@param img_Cr_out Chroma Cr unpadded.
*/
void extract_picture(int xsize, int ysize, int edge, int Crop, unsigned char img_luma_in[], unsigned char img_Cb_in[]
		, unsigned char img_Cr_in[],  unsigned char img_luma_out[],unsigned char img_Cb_out[], unsigned char img_Cr_out[]){


	int i,j;
	
	int Right = Crop & 0xff;
	int Left  = (Crop >> 8) & 0xff;
	int Bottom  = (Crop >> 16) & 0xff;
	int Top  = (Crop >> 24) & 0xff;
	int Xsize = xsize + 2 * edge + Right;

	const int offset_L = ((Xsize << 4) + edge);
	const int offset_C = ((Xsize << 2) + edge/2);
#ifdef MMX
	#ifdef SSE


	//problem with the player

	const int y_limit = (Ysize - 2*edge);
	const int x_limit = (Xsize - 2*edge) >> 4;


	__m128i *im_in = (__m128i *) (&img_luma_in[offset_L]);
	__m128i *im_in_Cb = (__m128i *) (&img_Cb_in[offset_C]);
	__m128i *im_in_Cr = (__m128i *) (&img_Cr_in[offset_C]);
	__m128i *im_out = (__m128i *) img_luma_out;
	__m128i *im_out_Cb = (__m128i *) img_Cb_out;
	__m128i *im_out_Cr = (__m128i *) img_Cr_out;
	const int W2 = Xsize >> 5;
	

	Xsize = Xsize >> 4;

	for (i = 0; i < y_limit; i++, im_in += Xsize){
		for (j = 0; j < x_limit; j++){
			_mm_storeu_si128(&im_out[i*(Xsize) + j], im_in[j]);
		}
	}


	for (i = 0; i < y_limit >> 1; i++, im_in_Cb += W2){
		for (j = 0; j < x_limit >> 1; j++){
			_mm_storeu_si128(&im_out_Cb[i * W2 + j],im_in_Cb[j]);
			_mm_storeu_si128(&im_out_Cr[i * W2 + j],im_in_Cr[i * W2 + j]);
			
		}
	}


	#else


	__m64 *im_in = (__m64 *) (&img_luma_in[offset_L]);
	__m64 *im_in_Cb = (__m64 *) (&img_Cb_in[offset_C]);
	__m64 *im_in_Cr = (__m64 *) (&img_Cr_in[offset_C]);
	__m64 *im_out = (__m64 *) img_luma_out;
	__m64 *im_out_Cb = (__m64 *) img_Cb_out;
	__m64 *im_out_Cr = (__m64 *) img_Cr_out;
	int y_limit = ysize;
	int x_limit = xsize >> 3;
	int stride = (Xsize - Right) >> 3;
	Xsize = Xsize >> 3;

	for (i = 0; i < y_limit; i++, im_in += Xsize){
		for (j = 0; j < x_limit; j++){
			im_out[i*(stride) + j] = im_in[j];
		}
	}


	Xsize = (Xsize + 1) >> 1;
	stride = stride >> 1;
	y_limit >>= 1;
	x_limit >>= 1;
	for (i = 0; i < y_limit; i++){
		for (j = 0; j < x_limit; j++){
			im_out_Cb[i * stride + j] = im_in_Cb[i * Xsize + j];
			im_out_Cr[i * stride + j] = im_in_Cr[i * Xsize + j];
		}
	}

		#ifndef POCKET_PC
			_mm_empty();    
		#endif  
	#endif
#else


	unsigned int *im_in = (unsigned int *) (&img_luma_in[offset_L]);
	unsigned int *im_in_Cb = (unsigned int *) (&img_Cb_in[offset_C]);
	unsigned int *im_in_Cr = (unsigned int *) (&img_Cr_in[offset_C]);
	unsigned int *im_out = (unsigned int *) img_luma_out;
	unsigned int *im_out_Cb = (unsigned int *) img_Cb_out;
	unsigned int *im_out_Cr = (unsigned int *) img_Cr_out;
	const int y_limit = ysize;
	const int x_limit = xsize >> 2;
	int stride = (Xsize - Right) >> 2;
	Xsize = (Xsize) >> 2;

	for (i = 0; i < y_limit; i++, im_in += Xsize){
		for (j = 0; j < x_limit; j++){
			im_out[i*(stride) + j] = im_in[j];
		}
	}


	Xsize = Xsize >> 1;
	stride = stride >> 1;

	for (i = 0; i < y_limit >> 1; i++){
		for (j = 0; j < x_limit >> 1; j++){
			im_out_Cb[i * stride + j] = im_in_Cb[i * Xsize + j];
			im_out_Cr[i * stride + j] = im_in_Cr[i * Xsize + j];
		}
	}

#endif
}





