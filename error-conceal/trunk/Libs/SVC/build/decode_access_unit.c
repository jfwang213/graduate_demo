#include "decode_access_unit.h"
#include "type.h"
#include "decoder_context.h"
#include "layer_utils.h"
#include "pic_list.h"
#include "SVCDisplay.h"
#include "memory.h"
#include "log.h"
#include "stdlib.h"
#include "ErrorConcealment.h"
#include "stdio.h"

void after_decode_au(decoder_context *pdecoder_context,display_context* pdisplay_context,
					 thread_context* pthread_context)
{
	MMO* mmo = pdecoder_context->mmo;
	int layer_id;
	int display_layer_id = get_layer_id(pdecoder_context->dq_to_display,pdecoder_context->layer_id_table);
	
	
	//error conceal
	{
		int t_layer_id,max_layer_id = -1;
		//error scenario 1
		//get highest layer receive nal
		for (t_layer_id = 0;pdecoder_context->layer_id_table[t_layer_id] != -1;t_layer_id++)
		{
			if (mmo->LayerMMO[t_layer_id].receive_new_nal)
			{
				max_layer_id = t_layer_id;
			}
		}

		if (max_layer_id >= 0)
		{
			printf("decode poc:%d\n",mmo->LayerMMO[max_layer_id].currentPic->poc);
			for (t_layer_id = max_layer_id+1;t_layer_id<=display_layer_id;t_layer_id++)
			{
				enhance_layer_error_conceal(t_layer_id,mmo,pdecoder_context,pthread_context,mmo->LayerMMO[max_layer_id].currentPic->poc);
			}
		}
		else
		{
			printf("detect one miss au!\n");
		}
	}
	//ref pic mark
	{
		for (layer_id = 0;layer_id<8;)
		{
			int highLayerId = get_same_dep_highest_layer_id(layer_id,mmo->layer_id_table);
			int highDqId = mmo->layer_id_table[highLayerId];
			if (highDqId > pdecoder_context->dq_to_display)
			{
				highDqId = pdecoder_context->dq_to_display;
			}
			highLayerId = get_layer_id(highDqId,mmo->layer_id_table);
			if (0 == mmo->LayerMMO[layer_id].dep_mmo->do_ref_pic_mark)
			{
				char tLayerId=layer_id,thighLayerId = highLayerId;
				if (mmo->LayerMMO[layer_id].dep_mmo->nal_ref_idc)
				{
					execute_ref_pic_marking(mmo->LayerMMO[layer_id].pcurr_sps,
						mmo,layer_id,highLayerId);
					if (mmo->LayerMMO[layer_id].dep_mmo->store_ref_base_pic_flag)
					{
						tLayerId = layer_id+1;
					}
					thighLayerId = highLayerId -1;
				}
				for (;tLayerId<=thighLayerId;tLayerId++)
				{
					release_pic_mv_memory(mmo->LayerMMO[tLayerId].currentPic);
					logStr("release mv memroy layerId:%d\n",tLayerId);
				}
			}
			mmo->LayerMMO[layer_id].dep_mmo->do_ref_pic_mark = 1;
			if (highDqId != pdecoder_context->dq_to_display)
			{
				layer_id = highLayerId + 1;
			}
			else
			{
				break;
			}
		}
	}
	
	//refresh dep variable
	{
		for (layer_id = 0;layer_id<=display_layer_id;)
		{
			int dep = pdecoder_context->layer_id_table[layer_id]>>4;
			int high_layer_id = get_same_dep_highest_layer_id(layer_id,pdecoder_context->layer_id_table);
			int temp_layer_id;
			for (temp_layer_id = layer_id;temp_layer_id<=high_layer_id;temp_layer_id++)
			{
				LAYER_MMO* temp_layer_mmo = &(mmo->LayerMMO[temp_layer_id]);
				if (temp_layer_mmo->receive_new_nal)
				{
					DEP_MMO* dep_mmo = temp_layer_mmo->dep_mmo;
					dep_mmo -> prev_frame_num_offset = temp_layer_mmo->slice -> frame_num_offset;
					dep_mmo -> prev_frame_num = temp_layer_mmo->slice -> frame_num;
					dep_mmo -> prevPicOrderCntLsb = temp_layer_mmo->slice -> pic_order_cnt_lsb;
					break;
				}
			}
			layer_id = high_layer_id+ 1;
		}
	}
	{
		int j;
		int base_layer_id = get_layer_id(pdecoder_context->dq_to_display>>4<<4,pdecoder_context->layer_id_table);

		DisplayOrderSvc(pdecoder_context,pdisplay_context->display_memory,pdisplay_context->xsize,
			pdisplay_context->ysize,pdisplay_context->crop,pdisplay_context->img_to_display,pdecoder_context->mmo);

		for (j = 0;pdecoder_context->layer_id_table[j] != -1;j++)
		{
			
			LIST_MMO* temp_current_pic;
			LAYER_MMO* tempLayerMmo = &(pdecoder_context->mmo->LayerMMO[j]);
			DEP_MMO* dep_mmo = tempLayerMmo->dep_mmo;
			if (!dep_mmo->active)
			{
				continue;
			}
			if (pdecoder_context->layer_id_table[j] > pdecoder_context->dq_to_display)
			{
				continue;
			}
			temp_current_pic = tempLayerMmo->currentPic;
			if (j != display_layer_id && (j !=base_layer_id  || !(tempLayerMmo->dep_mmo->store_ref_base_pic_flag)))
			{
				//not display layer release residual and sample memory
				release_pic_pic_memory(temp_current_pic);
				logStr("release all layerId:%d\n",j);
			}
			else
			{
				//display layer release residual memory
				release_pic_residu_memory(temp_current_pic);
				logStr("release residu layerId:%d\n",j);
			}
		}
	}
	//
	for (layer_id = 0;pdecoder_context->layer_id_table[layer_id] != -1;layer_id++)
	{
		if (mmo->LayerMMO[layer_id].slice)
		{
			free(mmo->LayerMMO[layer_id].slice);
		}
		if (mmo->LayerMMO[layer_id].nal)
		{
			free(mmo->LayerMMO[layer_id].nal);
		}
		mmo->LayerMMO[layer_id].slice = NULL;
		mmo->LayerMMO[layer_id].nal = NULL;
	}
}


void before_decoder_au(decoder_context* pdecoder_context,display_context* pdisplay_context)
{
	int layer_id;
	MMO* mmo = pdecoder_context->mmo;
	pdecoder_context->base_layer_lost = 0;
	for (layer_id = 0;pdecoder_context->layer_id_table[layer_id] != -1;layer_id++)
	{
		mmo->LayerMMO[layer_id].receive_new_nal = 0;
		mmo->LayerMMO[layer_id].slice = NULL;
		mmo->LayerMMO[layer_id].nal = NULL;
		mmo->LayerMMO[layer_id].error_concealment_done = 0;
	}
	for (layer_id = 0;layer_id<NUM_DEP_LAYERS;layer_id++)
	{
		mmo->DepMMO[layer_id].nal_ref_idc = 0;
		mmo->DepMMO[layer_id].store_ref_base_pic_flag = 0;
		mmo->DepMMO[layer_id].do_ref_pic_mark = 1;
		mmo->DepMMO[layer_id].receive_one_layer = 0;
		mmo->DepMMO[layer_id].frame_num_gap_check = 0;
		mmo->DepMMO[layer_id].active = 0;
		mmo->DepMMO[layer_id].poc_cal_done = 0;
	}
	pdisplay_context->img_to_display[0] = 0;
}