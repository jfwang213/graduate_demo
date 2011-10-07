#ifndef _SLICE_LAYER_IN_SCALABLE_EXTENSION_RBSP_H_
#define _SLICE_LAYER_IN_SCALABLE_EXTENSION_RBSP_H_

void slice_data_in_scalable_extension_cavlc ( const int size_mb, const unsigned char *Data, const int NalBytesInNalunit, 
											 const int *Position, NAL *Nal, const SPS *Sps, PPS *Pps, const VLC_TABLES *vlc, 
											 SLICE *Slice,  DATA *TabBlock, RESIDU *TabResidu, unsigned char *EndOfSlice,MMO *mmo);

void SliceCabac(const int size_mb, unsigned char *data, int NalBytesInNalunit, int *position, NAL *Nal, const SPS *Sps, PPS *Pps, 
				short *mv_cabac_l0, short *mv_cabac_l1, short *ref_cabac_l0, short *ref_cabac_l1, 
				SLICE *Slice,  DATA *Tab_block, RESIDU *TabResidu, unsigned char *EndOfSlice,MMO* mmo);

#endif