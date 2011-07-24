#include "loss_utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "layer_utils.h"
#include "decoder_context.h"
#include "AccessUnit.h"
#include "free_data.h"
#include "time.h"


FILE* loss_write_file = NULL;
FILE* loss_read_file = NULL;
int loss_from_file = 0;
int loss_file_end = 0;
int read_au_id,read_dq_id;
void loss_init_write()
{
	loss_write_file = fopen("loss_log.txt","w");
	srand((int)time(0));
}

void loss_init_read(char* file_name)
{
	loss_read_file = fopen(file_name,"r");
	loss_from_file = 1;
	loss_file_end = 0;
	loss_file_end = !(loss_log_read(&read_au_id,&read_dq_id));
}
void loss_uninit()
{
	if (loss_write_file)
	{
		fclose(loss_write_file);
	}
	if (loss_read_file)
	{
		fclose(loss_read_file);
	}
}

void loss_log_write(int au_id,int dq_id)
{
	fprintf(loss_write_file,"%d %d\n",au_id,dq_id);
	fflush(loss_write_file);
}


int loss_log_read(int *au_id,int *dq_id)
{
	return fscanf(loss_read_file,"%d %d\n",au_id,dq_id) != EOF;
}

double lost_rt[8] = {0.1,0.1,0.1,0.1,0,0,0,0};

void rtp_loss(int dq_id,access_unit* au)
{
	NAL* temp = NULL,*next;
	while (temp = get_next_nal(au,temp))
	{
		if (temp->DqId==dq_id)
		{
			next = get_pre_nal(au,temp);
			remove_one(temp->nal_list);
			free(temp->pSlice);
			free_nal(temp);
			temp = next;
			au->nal_num--;
		}
	}
}

void loss_au(access_unit* au,decoder_context* pdecoder_context,int au_id)
{
	int start_layer_id = 0;
	int display_layer_id = get_layer_id(pdecoder_context->dq_to_display,pdecoder_context->layer_id_table);
	int i;
	if (!loss_from_file)
	{
		for (i = start_layer_id;i<=display_layer_id;i++)
		{
			double t_num = rand();
			t_num /= RAND_MAX;
			if (t_num<lost_rt[i])
			{
				rtp_loss(pdecoder_context->layer_id_table[i],au);
				loss_log_write(au_id,pdecoder_context->layer_id_table[i]);
			}
		}
	}
	else
	{
		while (read_au_id == au_id)
		{
			rtp_loss(read_dq_id,au);
			loss_file_end = !(loss_log_read(&read_au_id,&read_dq_id));
			if (loss_file_end)
			{
				read_au_id = -1;
			}
		}
	}
	
}