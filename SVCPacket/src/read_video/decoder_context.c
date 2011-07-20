#include "decoder_context.h"
#include "pps.h"
#include "sps.h"
#include "stdlib.h"

void init_decoder_context(decoder_context* pdecoder_context)
{
	init_pps(pdecoder_context->all_pps);
	init_sps(pdecoder_context->all_sps);
	pdecoder_context->DqIdMax = 0;
	pdecoder_context->DependencyIdMax = 0;
	pdecoder_context->MaxNumLayer = 0;
	pdecoder_context->MaxTemporalId = 0;
}

