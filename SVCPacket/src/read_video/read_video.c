#include "read_video.h"
#include "stdlib.h"
#include "string.h"
#include "AccessUnit.h"
#include "stdio.h"
#include "bitstream.h"
#include "nal_unit_header_svc_extension.h"
#include "svc_type_util.h"
#include "slice_header_svc.h"
#include "sps.h"
#include "pps.h"
#include "nal.h"
#include "seq_parameter_set_svc_extension.h"

#define BUFFER_SIZE 1013760

FILE* video_file = 0;

static int file_size;
static int to_read_addr;
static int buffer_free_size;
static unsigned char buffer[BUFFER_SIZE];


static char is_file_end = 0;
static access_unit next_access_unit[1];
int File_size ( FILE *f )
{
	long    oldfp, fsize ;

	oldfp = ftell(f);
	if ( oldfp < 0L ) 
		return -1 ;
	if ( 0 != fseek(f, 0, SEEK_END) ) 
		return -1 ;
	fsize = ftell(f);
	if ( fsize < 0 ) 
		return -1 ;
	if ( 0 != fseek(f, oldfp, SEEK_SET) ) 
		return -1 ;
	return fsize ;
}

access_unit* get_one_access_unit(SPS* all_sps,PPS* all_pps,decoder_context *pdecoder_context)
{
	int i;
	NAL* next_nal = NULL;
	NAL* prefix_nal = NULL;
	access_unit* res = NULL;
	res = (access_unit*)malloc(sizeof(access_unit));
	init_access_unit(res);
	if (!is_blank(next_access_unit->plist_head))
	{
		next_nal = NULL;
		while (next_nal = get_next_nal(next_access_unit,next_nal))
		{
			remove_one(next_nal->nal_list);
			put_nal(res,next_nal);
			if (prefix_nal)
			{
				prefix_nal = NULL;
			}
			if (14 == next_nal->NalUnitType)
			{
				prefix_nal = next_nal;
			}
			next_nal = NULL;
		}
	}
	while (next_nal = get_next_nal_from_file(pdecoder_context))
	{
		char end_of_slice;
		NalUnitSVC(next_nal->prbsp_memory,next_nal);
		if(prefix_nal)
		{
			copy_nal_from_prefix(prefix_nal,next_nal);
		}
		prefix_nal = NULL;
		switch (next_nal->NalUnitType)
		{
		case 14:
			NalUnitData(next_nal->prbsp_memory, next_nal->current_position, next_nal, &end_of_slice);
			next_nal->LayerId = 0;
			if (-1 == next_nal->layer_id_table[0])
			{
				next_nal->layer_id_table[0] = 0;
			}
			break;
		case 20:
			NalUnitData(next_nal->prbsp_memory, next_nal->current_position, next_nal, &end_of_slice);
			for (i = 0;i<8;i++)
			{
				if (next_nal->layer_id_table[i] == next_nal->DqId)
				{
					next_nal->LayerId = i;
					break;
				}
				else if (-1 == next_nal->layer_id_table[i])
				{
					next_nal->layer_id_table[i] = next_nal->DqId;
					next_nal->LayerId = i;
					break;
				}
			}
			if (8 == i)
			{
				next_nal->LayerId = 0;
			}
			break;
		}
		if (14 == next_nal->NalUnitType)
		{
			prefix_nal = next_nal;
			put_nal(res,next_nal);
			continue;
		}
		if (is_vcl_nal(next_nal))
		{
			slice_header_svc_cut(next_nal->prbsp_memory,all_sps,all_pps,next_nal->current_position,next_nal->pSlice,
				next_nal);
		}
		else
		{
			switch (next_nal->NalUnitType)
			{
			case 6 : {
				break; }
			case 7 : {
				seq_parameter_set(next_nal->prbsp_memory, all_sps);
				break; }
			case 8 : {/* case_21 */
				pic_parameter_set(next_nal, all_pps,all_sps);
				break; }/* case_21 */
			case 15 : {/* case_9 */
				subset_sps(next_nal->prbsp_memory, next_nal->nal_byte_size, all_sps, 
					next_nal);
				break; }/* case_7 */
			}
			if ((9 == next_nal->NalUnitType || 6 == next_nal->NalUnitType)&&
				(!is_blank(res->plist_head)))
			{
				put_nal(next_access_unit,next_nal);
				break;
			}
			else
			{
				put_nal(res,next_nal);
				continue;
			}
		}
		if (is_end_access_unit(res,next_nal))
		{
			put_nal(next_access_unit,next_nal);
			break;
		}
		else
		{
			put_nal(res,next_nal);
		}
	}
	if (is_blank(res->plist_head))
	{
		return NULL;
	}
	//remove prefix back to next access unit
	next_nal = NULL;
	while (next_nal = get_pre_nal(res,next_nal))
	{
		if (14 == next_nal->NalUnitType)
		{
			res->nal_num--;
			remove_one(next_nal->nal_list);
			put_first_nal(next_access_unit,next_nal);
			next_nal = NULL;
		}
		else
		{
			break;
		}
	}
	return res;
}

void assign_flag_to_access_unit(access_unit* punit)
{
	NAL* nal = NULL,*next_vcl_nal;
	while (nal = get_next_nal(punit,nal))
	{
		if (is_vcl_nal(nal))
		{
			next_vcl_nal = nal;
			while (next_vcl_nal = get_next_nal(punit,next_vcl_nal))
			{
				if (is_vcl_nal(next_vcl_nal))
				{
					break;
				}
			}
			if (next_vcl_nal)
			{
				nal->NextDqId = next_vcl_nal->DqId;
				if (next_vcl_nal->DqId != nal->DqId)
				{
					nal->bFinishLayerRepresent = 1;
				}
				else
				{
					nal->bFinishLayerRepresent = 0;
				}
				if (next_vcl_nal->DependencyId != nal->DependencyId)
				{
					nal->bFinishDependenceRepresent = 1;
				}
				else
				{
					nal->bFinishDependenceRepresent = 0;
				}
				nal->bFinishAccessUnit = 0;

			}
			else
			{
				nal->bFinishAccessUnit = 1;
				nal->NextDqId = 0;
				break;
			}
		}
	}
}
NAL* get_next_nal_from_file(decoder_context *pdecoder_context)
{
	int read_size,i,byte_rbsp_num;
	NAL* res_nal;
	int rbsp_start,rbsp_internal;
	if (!is_file_end)
	{
		read_size = fread(&(buffer[BUFFER_SIZE-buffer_free_size]),1,buffer_free_size,video_file);
		to_read_addr += read_size;
		buffer_free_size-=read_size;
		if (buffer_free_size>4)
		{
			if(!(buffer[BUFFER_SIZE-buffer_free_size-1] == 1&&
				buffer[BUFFER_SIZE-buffer_free_size-2] == 0&&
				buffer[BUFFER_SIZE-buffer_free_size-3] == 0))
			{
				buffer[BUFFER_SIZE-buffer_free_size] = 0;
				buffer[BUFFER_SIZE-buffer_free_size+1] = 0;
				buffer[BUFFER_SIZE-buffer_free_size+2] = 0;
				buffer[BUFFER_SIZE-buffer_free_size+3] = 1;
				buffer_free_size-=4;
			}
			is_file_end = 1;
		}
	}
	if (BUFFER_SIZE-buffer_free_size<=4)
	{
		return NULL;
	}
	rbsp_internal = search_start_code(buffer,&rbsp_start);
	res_nal = (NAL*)malloc(sizeof(NAL));
	init_nal(res_nal,pdecoder_context);
	res_nal->prbsp_memory = (unsigned char*)malloc(rbsp_internal+20);
	//20 is for cavlc read ue buffer.
	byte_rbsp_num = 0;
	res_nal->nrbsp_memory_size = rbsp_internal;
	for ( i = rbsp_start ; i < rbsp_internal + rbsp_start; i++ ) {
		res_nal->prbsp_memory [byte_rbsp_num++] = buffer [i];
		if ( i + 2 < (rbsp_internal + rbsp_start) && show3Bytes_Nal(buffer, i) == 3 ) {
			res_nal->prbsp_memory [byte_rbsp_num++] = buffer [i++] ;
			i ++ ;
		}
	}
	res_nal->nal_byte_size = byte_rbsp_num;
	buffer_free_size += rbsp_internal+rbsp_start;
	memcpy(buffer,&(buffer[rbsp_internal+rbsp_start]),BUFFER_SIZE-buffer_free_size);
	return res_nal;
}

char read_video_init(const char* fileName)
{
	if (NULL == fileName)
	{
		printf("can't find fileName from parameters\n");
		return 0;
	}
	video_file = fopen(fileName,"rb");
	if (NULL == video_file)
	{
		printf("can't open file %s\n",fileName);
		return 0;
	}
	file_size = File_size(video_file);
	buffer_free_size = BUFFER_SIZE;
	to_read_addr = 0;

	init_access_unit(next_access_unit);
	return 1;
}


void close_read_video()
{
	if (video_file)
	{
		fclose(video_file);
	}
}