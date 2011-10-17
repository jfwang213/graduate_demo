#include "type.h"
#include "main_data.h"
#include "svc_type.h"
#include "nal.h"

#include "log.h"
#include "free_data.h"
#include "SwitchAdress.h"

#include "extract_picture.h"
#include "mysdl.h"
#include "logMbInfo.h"
#include "set_layer.h"
#include "sps.h"
#include "FinishFrameSVC.h"
#include "SVCDisplay.h"
#include "memory.h"
#include "assert.h"
#include "AccessUnit.h"
#include "pps.h"
#include "sps.h"
#include "read_video.h"
#include "decode_base_layer_nal.h"
#include "decode_svc_layer_nal.h"
#include "svc_type_util.h"
#include "nal_unit_header_svc_extension.h"
#include "init_Ptr_Fction.h"
#include "cavlc.h"
#include "cal_time.h"
#include "write_yuv_file.h"
#include "layer_utils.h"
#include "decode_access_unit.h"
#include "loss_utils.h"

/* 2010-11-05 13:15:55, application decoder_svc, processor PC type=pentiumOS */

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#define uchar unsigned char
#define ushort unsigned short
#define ulong unsigned long
#define uint unsigned int
#define prec_synchro int
#define stream unsigned char
#define image_type unsigned char
#define dpb_type unsigned char

#include "decoder_context.h"
#include "mb_svc.h"
#include "cal_time.h"

#ifdef _CAL_TIME_
double parse_time,decode_time,display_time,display_time2;
double layer_time[8][2];
#endif
decoder_context pdecoder_context[1];
thread_context pthread_context[1];
display_context pdisplay_context[1];



void print_usage()
{
	printf("svc [-h264 filename] [-network] -layer layerdepId<<4+qualityId -write_yuv yuv_file_name\n");
}
char silent_flag = 0;
char loss_flag = 0;
int read_loss_file = 0;
void parse_argv(int argc,char* argv[])
{
	int i = 1;
	for (;i<argc;i++)
	{
		if(strcmp("-silent",argv[i]) == 0)
		{
			silent_flag = 1;
			printf("in silent mode\n");
		}
		if (strcmp("-loss",argv[i]) == 0)
		{
			loss_flag = 1;
			printf("in loss mode!\n");
		}
		if (strcmp("-read-loss",argv[i]) == 0)
		{
			read_loss_file = 1;
			printf("read loss file!\n");
		}
	}
}
int main(int argc, char* argv[]) { /* for link with C runtime boot */
	char yuvFileName[260];
	access_unit* current_access_unit;
	NAL* current_nal = NULL;
	NAL* last_nal = NULL;
	NAL* prefix_nal = NULL;
	NAL* next_nal = NULL;
	
	char isEnd = 0;
	char isCheck = -1;
	int checkNum = 0;
	int accessId = 0;
	int write_yuv = parse_arg_write_yuv(argc,argv,yuvFileName);
#ifdef _CAL_TIME_
	int i;
	parse_time = 0;
	decode_time = 0;
	display_time = 0;
	display_time2 = 0;
	memset(layer_time,0,sizeof(layer_time));
	time_init();
#endif
	if (write_yuv)
	{
		if(!init_write_yuv(yuvFileName))
		{
			printf("init yuv failed!\n");
			print_usage();
			return -1;
		}
	}
	parse_argv(argc,argv);
	loss_init_write();
	if (read_loss_file)
	{
		loss_init_read("F:\\code\\ParallelSvc2\\trunk\\Libs\\SVC\\bin\\loss_log_1.txt");
	}
	log_init();
	logStr("start log\n");
	log_flush();
	init_memory_cache();

	if(!read_video_init(argc,argv))
	{
		print_usage();
		return;
	}
	get_layer(argc,argv,pdecoder_context);
	decoderh264_init(MAX_WIDTH, MAX_HEIGHT);
	init_decoder_context(pdecoder_context);
	init_thread_context(pthread_context,pdecoder_context);
	Init_SDL(16, MAX_WIDTH, MAX_HEIGHT);
	init_svc_vectors(pdecoder_context->svc_function);
	decode_init_vlc(pdecoder_context->vlc_table);
	init_vectors(pdecoder_context->pred_function->baseline_vectors);
	
	
	for(;!isEnd;){ /* loop_2 */ 
		if (IsSDLQuit())
		{
			logStr("quit\n");
			
			break;
		}
		current_access_unit = get_one_access_unit(pdecoder_context->all_sps,pdecoder_context->all_pps,pdecoder_context);
		if (NULL == current_access_unit)
		{
			break;
		}
		printf("accessId:%d\n",accessId);
		remove_useless_layer(pdecoder_context->dq_to_display,current_access_unit);
		if (accessId>5 && loss_flag)
		{
			loss_au(current_access_unit,pdecoder_context,accessId);
		}
		
		if (0 == current_access_unit->nal_num)
		{
			accessId++;
			continue;
		}
		logStr("accessid:%d\n",accessId);
		assign_flag_to_access_unit(current_access_unit);
		before_decoder_au(pdecoder_context, pdisplay_context);
		while (next_nal = get_next_nal(current_access_unit,NULL))
		{
			if (is_vcl_nal(next_nal))
			{
				last_nal = current_nal;
			}
			current_nal = next_nal;
			if (NULL != last_nal && is_vcl_nal(current_nal) && current_nal->QualityId != 0)
			{
				if (last_nal->DependencyId == current_nal->DependencyId)
				{
					current_nal->StoreRefBasePicFlag = last_nal->StoreRefBasePicFlag;
					current_nal->SpatialScalability = last_nal->SpatialScalability;
				}
				else
				{
					current_nal->StoreRefBasePicFlag = 0;
					if (current_nal->QualityId >= 1)
					{
						current_nal->SpatialScalability = 0;
					}
					else
					{
						current_nal->SpatialScalability = 1;
					}
				}
			}
			remove_one(next_nal->nal_list);
			
			NalUnitSVC(current_nal->prbsp_memory,current_nal);
			if (5 == current_nal->NalUnitType)
			{
				switch_layer(pdecoder_context->select_layer,current_nal,pdecoder_context);
			}
			logStr("layer id:%d\n",current_nal->LayerId);
			switch (current_nal->NalUnitType)
			{
			case 1:
				decode_base_layer_nal(current_nal,pdecoder_context,pthread_context,pdisplay_context);
				break;
			case 5:
				decode_base_layer_idr_nal(current_nal,pdecoder_context,pthread_context,pdisplay_context);
				break;
			case 14:
				decode_prefix_nal(current_nal,pdecoder_context,pthread_context);
				break;
			case 20:
				decode_svc_layer_nal(current_nal,pdecoder_context,pthread_context,pdisplay_context);
				break;
			default:
				break;
			}
			if (is_vcl_nal(current_nal))
			{
				log_slice_info(current_nal,pdecoder_context,pthread_context);
			}
			//log
			if (current_nal->bFinishAccessUnit && is_vcl_nal(current_nal))
			{
				log_memory_state(current_nal,pdecoder_context->mmo);
			}
		}
		after_decode_au(pdecoder_context,pdisplay_context,pthread_context);
		switch (pdisplay_context->img_to_display[0]) 
		{ 
		case 1 : 
			{
				int XDIM = ((int *) pdisplay_context->display_image)[0] = *pdisplay_context->xsize;
				int YDIM = ((int *) pdisplay_context->display_image)[1] = *pdisplay_context->ysize;
				uchar *Y = pdisplay_context->display_image + 8;
				uchar *U = Y + (XDIM + 32) * YDIM;
				uchar *V = U + (XDIM + 32) * YDIM/4;
				char op;
				if (write_yuv)
				{
					write_one_frame(pdisplay_context->display_memory->display_y,
						pdisplay_context->display_memory->display_u,
						pdisplay_context->display_memory->display_v,XDIM,YDIM);
				}
#ifdef _CAL_TIME_
				time_start();
#endif
				extract_picture(XDIM, YDIM, 16, pdisplay_context->crop[0], 
					pdisplay_context->display_memory->display_y, pdisplay_context->display_memory->display_u, 
					pdisplay_context->display_memory->display_v,
					Y, U, V);
				if (pdisplay_context->display_memory->to_release)
				{
					release_display_memory(pdisplay_context->display_memory);
				}
				pdisplay_context->img_to_display[0] = 0;
#ifdef _CAL_TIME_
				display_time += time_end();
				time_start();
#endif
				SDL_Display(16, XDIM, YDIM, Y, U, V);
				if (!silent_flag&&isCheck != -1 && (isCheck || checkNum >= 10))
				{
					checkNum = 0;
					isCheck = 1;
					op = getKeyInput();
					if(-1 == op)
					{
						isEnd = 1;
					}
					else if ('b' == op)
					{
						isCheck = 0;
					}
					else if ('c' == op)
					{
						isCheck = -1;
					}
				}
				checkNum++;
#ifdef _CAL_TIME_
				display_time2+=time_end();
#endif
				break; 
			}
		case 2 : 
			{
				assert(0);
				break; 
			}
		}
		accessId++;
		free_access_unit(current_access_unit);
	} 
#ifdef _CAL_TIME_
	printf("display_time1:%f\t display_time2:%f\n",display_time,display_time2);
	for (i = 0;i<8;i++)
	{
		if (layer_time[i][0]<0.01)
		{
			break;
		}
		printf("layer:%d\tparse_time:%f\tdecode_time:%f\n",i,layer_time[i][0],layer_time[i][1]);
	}
#endif
	CloseSDLDisplay();
	free_mmo(pdecoder_context->mmo);
	free_pps_memory(pdecoder_context->all_pps);
	log_exit();
	free_memory_cache();
	close_read_video();
	loss_uninit();
	if (write_yuv)
	{
		close_write_yuv();
	}
	printf("end\n");
	return(0);
} /* end of main */


