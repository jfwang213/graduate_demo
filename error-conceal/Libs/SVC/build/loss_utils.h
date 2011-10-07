#ifndef _LOSS_UTILS_H_
#define _LOSS_UTILS_H_
#include "decoder_context.h"
#include "AccessUnit.h"




void rtp_loss(int dq_id,access_unit* au);
void loss_init_write();
void loss_init_read(char* file_name);
void loss_uninit();
void loss_log_write(int au_id,int dq_id);
int loss_log_read(int *au_id,int *dq_id);
void rtp_loss(int dq_id,access_unit* au);
void loss_au(access_unit* au,decoder_context* pdecoder_context,int au_id);


#endif