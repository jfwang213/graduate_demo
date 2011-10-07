#ifndef _READ_VIDEO_H_
#define _READ_VIDEO_H_

#include "AccessUnit.h"
#include "decoder_context.h"
#include "type.h"
typedef NAL* (*get_next_nal_method)(decoder_context *);

char read_video_init(int argc,char *argv[]);

access_unit* get_one_access_unit(SPS* all_sps,PPS* all_pps,decoder_context *pdecoder_context);
NAL* get_next_nal_from_file(decoder_context *pdecoder_context);
void assign_flag_to_access_unit(access_unit* punit);

void close_read_video();
#endif