#ifndef _SLICE_DATA_CAVLC_H_
#define _SLICE_DATA_CAVLC_H_


void slice_base_layer_cavlc ( unsigned char *ai_pcData, const int ai_piNalBytesInNalunit, const int *aio_piPosition, const SPS *Sps, 
							 PPS *Pps, VLC_TABLES *Vlc, LIST_MMO *Current_pic, LIST_MMO *ai_pRefPicListL1, 
							 NAL *Nal, SLICE *Slice,  DATA Block[], 
							 RESIDU *picture_residu, unsigned char *EndOfSlice,MMO* mmo);
int B_cavlc ( const PPS *pps, RESIDU *picture_residu, const unsigned char *ai_pcData,  const int ai_piNalBytesInNalunit, 
			 const SPS *Sps , DATA aio_tstBlock [], SLICE *Slice, int *aio_piPosition,  const VLC_TABLES *Vlc, 
			 unsigned char aio_tiSlice_table [], const unsigned char ai_tiMbToSliceGroupMap [] , unsigned char *EndOfSlice, LIST_MMO *RefListl1, 
			 short *mvl0_io, short *mvl1_io, short *refl0_io, short *refl1_io, short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1);
int P_cavlc(const PPS *pps, RESIDU *picture_residu, const unsigned char *ai_pcData, const int ai_piNalBytesInNalunit, 
			const SPS *Sps, DATA aio_tstBlock [], SLICE *Slice, int *aio_piPosition, const VLC_TABLES *Vlc, 
			unsigned char Slice_table [],  const unsigned char SliceGroupMap [], unsigned char *EndOfSlice, short *mvl0_io, short *refl0_io) ;

#endif