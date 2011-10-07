#ifndef _NETWORK_H_
#define _NETWORK_H_
#include "type.h"
#include "decoder_context.h"

int network_init();
NAL* get_one_nal_from_network(decoder_context *pdecoder_context);
void network_close();


#endif