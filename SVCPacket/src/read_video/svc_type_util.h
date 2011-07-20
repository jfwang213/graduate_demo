#ifndef _SVC_TYPE_UTIL_H_
#define _SVC_TYPE_UTIL_H_


#include "svc_type.h"
#include "decoder_context.h"

char is_vcl_nal(NAL* pNal);

void init_nal(NAL* pNal,decoder_context* pdecoder_context);

void copy_nal_from_prefix(NAL *prefix_nal,NAL *current_nal);

void free_nal(NAL* pNal);

#endif

