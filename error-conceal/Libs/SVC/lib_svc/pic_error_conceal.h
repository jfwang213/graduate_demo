#ifndef _PIC_ERROR_CONCEAL_H_
#define _PIC_ERROR_CONCEAL_H_

#include "type.h"

void pic_error_conceal_mp(NAL* nal,short* mv_l0_l0,short* ref_l0_l0,short* mv_l0,short* ref_l0,
						  int mb_num,int mb_width_in_mbs);

#endif