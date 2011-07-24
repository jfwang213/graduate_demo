#ifndef _CABACIMACROBLOCK_H_
#define _CABACIMACROBLOCK_H_


int CabacIPartioning (const NAL * Nal,RESIDU * base_picture_residu,RESIDU *picture_residu, SLICE *slice, DATA Tab_block[],	DATA *block, 
					  unsigned char non_zero_count_cache [], unsigned char slice_table [],
					  int iCurrMbAddr , CABACContext *cabac, unsigned char *cabac_state,
					  CABAC_NEIGH *neigh,const short x,const short y, int constrained_intra_pred_flag,
					  int dct_allowed, int *last_QP, int *mb_qp_delta);

#endif