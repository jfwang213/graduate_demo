#ifndef _SLICE_DATA_CABAC_H_
#define _SLICE_DATA_CABAC_H_

void	slice_base_layer_cabac ( unsigned char *data,  int NalBytesInNalunit, int *aio_piPosition, SPS *sps, PPS *pps, 
								LIST_MMO *Current_pic, LIST_MMO *ai_pRefPicListL1, NAL *Nal, 
								short mv_cabac_l0[], short mv_cabac_l1[], short ref_cabac_l0[], short ref_cabac_l1[], 
								SLICE *slice, DATA Tab_block[], RESIDU *picture_residu, 
								unsigned char *end_of_slice,MMO* mmo);
int P_cabac (RESIDU *picture_residu, unsigned char *data,int NalBytesInNalunit, SPS *sps, PPS *pps, DATA block [], 
			 SLICE *slice, int *aio_piPosition, unsigned char slice_table [], unsigned char MbToSliceGroupMap [], short mv_cabac_l0[], 
			 short ref_cabac_l0[], unsigned char *EndOfSlice, short *mvl0_io, short *refl0_io);
int B_cabac ( RESIDU *picture_residu, unsigned char *data,int NalBytesInNalunit, SPS *sps, PPS *pps, DATA block [], 
			 SLICE *slice, int *aio_piPosition, unsigned char slice_table [], unsigned char MbToSliceGroupMap [], short mv_cabac_l0[], 
			 short mv_cabac_l1[], short ref_cabac_l0[], short ref_cabac_l1[], unsigned char *EndOfSlice, LIST_MMO * ai_pRefPicListL1, 
			 short *mvl0_io, short *mvl1_io, short *refl0_io, short *refl1_io, int long_term, int slice_type, 
			 short *mvl1_l0, short *mvl1_l1, short *refl1_l0, short *refl1_l1);

#endif