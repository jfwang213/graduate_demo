#ifndef _SLICE_HEADER_SVC_H_
#define _SLICE_HEADER_SVC_H_


#include "type.h"

int slice_header_svc(unsigned char *data, SPS *sps_id, PPS *pps_id,
					  unsigned char *entropy_coding_flag, W_TABLES *quantif, LIST_MMO *Current_pic, 
					  SPS *sps, PPS *pps, int *position, SLICE *Slice, MMO *Mmo, 
					  LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[], NAL *Nal, unsigned char *end_of_slice) ;


//for initial accessunit
int slice_header_svc_cut(unsigned char *data, SPS *sps, PPS *pps, int *position, 
						 SLICE *Slice, NAL *Nal);
#endif