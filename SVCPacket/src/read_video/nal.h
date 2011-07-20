
#ifndef NAL_H
#define NAL_H

#include "type.h"
#include "bitstream.h"





int search_start_code ( unsigned char *data, int *offset );
void Nal_unit (unsigned char *data_in, int *nal_unit_type, int *nal_ref_idc);
int get_NalBytesInNalunit ( unsigned char *data,  unsigned char *rbsp_byte, int *NalInRBSP);
void decoderh264_init(int pic_width, int pic_height);
#endif
