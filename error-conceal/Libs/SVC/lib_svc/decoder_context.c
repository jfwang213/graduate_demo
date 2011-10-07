#include "decoder_context.h"
#include "pps.h"
#include "sps.h"
#include "init_Ptr_Fction.h"
#include "init_data.h"
#include "stdlib.h"
#include "cavlc.h"
#include "mb_svc.h"
#include "svc_type_util.h"

void init_decoder_context(decoder_context* pdecoder_context)
{
	int i,j;
	init_pps(pdecoder_context->all_pps);
	init_sps(pdecoder_context->all_sps);
	init_mmo(max_layer,pdecoder_context->mmo,pdecoder_context);
	init_svc_vectors(pdecoder_context->svc_function);
	init_vectors(pdecoder_context->pred_function->baseline_vectors);
	decode_init_vlc(pdecoder_context->vlc_table);
	for (i = 0;i<8;i++)
	{
		pdecoder_context->layer_id_table[i] = -1;
		for (j = 0;j<8;j++)
		{
			pdecoder_context->UsedSpatial[i][j] = -1;
		}
	}
	pdecoder_context->DqIdMax = 0;
	pdecoder_context->DependencyIdMax = 0;
	pdecoder_context->MaxNumLayer = 0;
	pdecoder_context->MaxTemporalId = 0;
}

void init_thread_context(thread_context* pthread_context,decoder_context *pdecoder_context)
{
	pthread_context->end_of_slice[0] = 0;
}
