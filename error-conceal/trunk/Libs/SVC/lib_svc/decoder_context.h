#ifndef _DECODER_CONTEXT_H_
#define _DECODER_CONTEXT_H_

#include "type.h"
#include "main_data.h"
#include "svc_type.h"

typedef struct
{
	DISPLAY_M display_memory[1];
	unsigned char display_image[one_pic_memory];
	int crop[1];
	int xsize[1];
	int ysize[1];
	int img_to_display[1];
	
}display_context;

typedef struct
{
	PPS all_pps[255];
	SPS all_sps[32];
	ConfigSVC stream_type[1];
	MMO mmo[1];
	SVC_VECTORS svc_function[1];
	MAIN_STRUCT_PF pred_function[1];
	VLC_TABLES vlc_table[1];
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
	char base_layer_lost;

}decoder_context;

typedef struct
{
	unsigned char end_of_slice[1];
	unsigned char entropy_flag[1];
	unsigned char layer_useful[1];
	W_TABLES current_quantif[1];
	int current_position[1];
	short mv_cabac_l0_o[MB_NUM*2*16];
	short mv_cabac_l1_o[MB_NUM*2*16];
	short ref_cabac_l0_o[MB_NUM*4];
	short ref_cabac_l1_o[MB_NUM*4];
	short svc_Upsampling_tmp[one_pic_sample];
	short svc_px[MAX_WIDTH];
	short svc_py[MAX_HEIGHT];
	short svc_xk16[MAX_WIDTH];
	short svc_xp16[MAX_WIDTH];
	short svc_yk16[MAX_HEIGHT];
	short svc_yp16[MAX_HEIGHT];
	DATA Block[MB_NUM];
	RESIDU residu[MB_NUM*NUM_LAYERS];
	SPS	current_sps_id[1];
	PPS	current_pps_id[1];
	
}thread_context;


void init_thread_context(thread_context* pthread_context,decoder_context *pdecoder_context);
void init_decoder_context(decoder_context* pdecoder_context);
#endif