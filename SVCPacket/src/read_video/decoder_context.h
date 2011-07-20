#ifndef _DECODER_CONTEXT_H_
#define _DECODER_CONTEXT_H_

#include "type.h"
#include "main_data.h"
#include "svc_type.h"


typedef struct
{
	PPS all_pps[255];
	SPS all_sps[32];
	ConfigSVC stream_type[1];
	int select_layer[1];
	int temporal_id[1];
	char layer_id_table[8];
	char UsedSpatial[8][8];
	int pic_parameter_id[8];
	int DqIdMax;
	int DependencyIdMax;
	int MaxTemporalId;
	int MaxNumLayer;
	unsigned char dq_to_display;

}decoder_context;

void init_decoder_context(decoder_context* pdecoder_context);
#endif