#ifndef _SVCDecoder_ietr_api_h
#define _SVCDecoder_ietr_api_h

#ifdef TI_OPTIM
#define WINAPI 
#else
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
	SVC_STATUS_ERROR	= -1,
	SVC_STATUS_OK	    = 0,	// no error and no frame ready
	SVC_IMAGE_READY     = 1,	// no error and image ready
	SVC_GHOST_IMAGE		= 2		// no image for chosen layer but image could be ready for other layers
};


#define    WINEXPORT WINAPI 


/*======================================================================================================
int SVCDecoder_init();
This method initializes the internal resources of the decoder
@return SVC_STATUS_OK if creation is successful
======================================================================================================*/
int WINEXPORT SVCDecoder_init();

/*======================================================================================================
int setLayer(int num_layer);
This method indicates to the decoder the number of the displayed layer.
This method can be called at any instant of the stream but resulting behavior is stable only when 
a IDR occurs.
@param num_layer is the number of the layer to decode (from 0 to NLayer - 1)
@return SVC_STATUS_OK or SVC_STATUS_ERROR
======================================================================================================*/
int WINEXPORT setLayer(int num_layer);

/*======================================================================================================
int decodeNAL(	unsigned char* nal, int nal_length, int* pWidth, int* pHeight, 
unsigned char** pY, unsigned char** pU, unsigned char** pV, int DqIdMax);
Use this method to give a new NAL Unit to the decoder. The NAL content (no start code)
is stored at the address given by the argument pNAL and the length of the NAL is provided 
by the argument nal_length. Memory of this nal buffer is managed by the application.
If the function returns the code SVC_IMAGE_READY, this means that an image is ready and in this case, 
the arguments pWidth, pHeight have been updated by the decoder and the image plans are contained at 
the addresses pY, PU, pV.
The image stored at these location is an image centered in a rectangle of size (*pWidth + 32, *pHeight + 32)
for Y and ((*pWidth + 32) / 2, (*pHeight + 32) / 2)) for U and V.
Memory of these image buffers is managed by the decoder.
In case of a SVC stream, the largest DqIq of the Access Unit should be given.

@param nal is the address of the buffer that contains the NAL unit
@param nal_length is the size of NAL Unit data
@param pWidth is a pointer on the image width updated by decoder
@param pHeight is a pointer on the image height updated by decoder
@param pY is a pointer address updated by the decoder with the address of the Y plan
@param pU is a pointer address updated by the decoder with the address of the U plan
@param pV is a pointer address updated by the decoder with the address of the V plan
@param DQIdMax is the largest DQId of the current access unit
@return SVC_STATUS_OK or SVC_STATUS_ERROR or SVC_IMAGE_READY or SVC_GHOST_IMAGE
======================================================================================================*/
int WINEXPORT decodeNAL(	unsigned char* nal, int nal_length, int* pWidth, int* pHeight, unsigned char** pY, unsigned char** pU, unsigned char** pV, int DqIdMax); 


/*======================================================================================================
int SVCDecoder_close();
This method releases the internal resources of the decoder
@return SVC_STATUS_OK if creation is successfull
=======================================================================================================*/
int WINEXPORT SVCDecoder_close();

#ifdef __cplusplus
}
#endif
#endif	// SVCDecoder_ietr_api
