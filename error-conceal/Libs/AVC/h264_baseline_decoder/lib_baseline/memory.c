#include "memory.h"
#include "type.h"
#include "stdlib.h"
#include "log.h"
#include "assert.h"

#define max_mem_size_num 10
#define max_mem_free_num 10

static int mem_cache_num[max_mem_size_num];
static void* mem_cache_point[max_mem_size_num][max_mem_free_num];
static int mem_cache_size[max_mem_size_num];
static char use_cache = 0;

void init_memory_cache()
{
	if (use_cache)
	{
		memset(mem_cache_num,0,sizeof(mem_cache_num));
		memset(mem_cache_size,255,sizeof(mem_cache_size));
		memset(mem_cache_point,0,sizeof(mem_cache_point));
	}
	
}


void free_memory_cache()
{
	int i,j;
	if (use_cache)
	{
		for (i = 0;i<max_mem_size_num;i++)
		{
			if (mem_cache_size[i]>=0)
			{
				for (j = 0;j<mem_cache_num[i];j++)
				{
					free(mem_cache_point[i][j]);
				}
			}
			else
			{
				break;
			}
		}
	}
}

void* allocate_memory(int size)
{
	void* res = NULL;
	//find cache
	int i;
	if (use_cache)
	{
		for (i = 0;i<size;i++)
		{
			if (mem_cache_size[i] == size)
			{
				if (mem_cache_num[i]>0)
				{
					mem_cache_num[i]--;
					res = mem_cache_point[i][mem_cache_num[i]];
					mem_cache_point[i][mem_cache_num[i]] = NULL;
					return res;
				}
				else
				{
					break;
				}
			}
			else if (-1 == mem_cache_size[i])
			{
				mem_cache_size[i] = size;
				break;
			}
		}
	}
	
	res = malloc((size_t)size);
	logStr("allocate memory %x\n",res);
	return res;
}

void release_memory(void* p,int size)
{
	int i;
	//assert(p);
	if (use_cache)
	{
		for (i = 0;i<max_mem_size_num;i++)
		{
			if (mem_cache_size[i] == size)
			{
				if (mem_cache_num[i]<max_mem_free_num)
				{
					mem_cache_point[i][mem_cache_num[i]] = p;
					mem_cache_num[i]++;
				}
				else
				{
					free(p);
				}
				return;
			} 
			else if (-1 == mem_cache_size[i])
			{
				break;
			}
		}
	}
	logStr("release memory %x\n",p);
	free(p);
}

void allocate_mv_memory(LIST_MMO* current_pic)
{
	current_pic->mv_memory_l0 = (short*)allocate_memory(current_pic->memory_size>>2);
	current_pic->mv_memory_l1 = (short*)allocate_memory(current_pic->memory_size>>2);
	current_pic->ref_memory_l0 = (short*)allocate_memory(current_pic->memory_size>>3);
	current_pic->ref_memory_l1 = (short*)allocate_memory(current_pic->memory_size>>3);
	assert(current_pic->mv_memory_l0);
	assert(current_pic->mv_memory_l1);
	assert(current_pic->ref_memory_l0);
	assert(current_pic->ref_memory_l1);
	memset(current_pic->ref_memory_l0,-1,current_pic->memory_size>>3);
	memset(current_pic->ref_memory_l1,-1,current_pic->memory_size>>3);
}

void allocate_pic_sample_memory(LIST_MMO* current_pic)
{
	current_pic->picture_memory_y = (unsigned char*)allocate_memory(current_pic->memory_size);
	current_pic->picture_memory_u = (unsigned char*)allocate_memory(current_pic->memory_size>>2);
	current_pic->picture_memory_v = (unsigned char*)allocate_memory(current_pic->memory_size>>2);
	assert(current_pic->picture_memory_y);
	assert(current_pic->picture_memory_u);
	assert(current_pic->picture_memory_v);
}

void allocate_picture_memory(LIST_MMO* current_pic)
{

	current_pic->residual_memory_y = (short*)allocate_memory(current_pic->memory_size<<1);
	current_pic->residual_memory_u = (short*)allocate_memory(current_pic->memory_size>>1);
	current_pic->residual_memory_v = (short*)allocate_memory(current_pic->memory_size>>1);

	logStr("allocate memory, poc:%d,\tmemory_size:%d\n",current_pic->poc,current_pic->memory_size);

	allocate_pic_sample_memory(current_pic);
	allocate_mv_memory(current_pic);
	assert(current_pic->residual_memory_y);
	assert(current_pic->residual_memory_u);
	assert(current_pic->residual_memory_v);
	
	log_flush();

}
void release_picture_memory(LIST_MMO* current_pic)
{
	release_pic_mv_memory(current_pic);
	release_pic_sample_memory(current_pic);
	release_pic_residu_memory(current_pic);
}


void release_pic_pic_memory(LIST_MMO* current_pic)
{
	release_pic_sample_memory(current_pic);
	release_pic_residu_memory(current_pic);
}
void release_pic_mv_memory(LIST_MMO* current_pic)
{
	release_memory(current_pic->mv_memory_l0,current_pic->memory_size>>2);
	release_memory(current_pic->mv_memory_l1,current_pic->memory_size>>2);
	release_memory(current_pic->ref_memory_l0,current_pic->memory_size>>3);
	release_memory(current_pic->ref_memory_l1,current_pic->memory_size>>3);
	current_pic->mv_memory_l0 = NULL;
	current_pic->mv_memory_l1 = NULL;
	current_pic->ref_memory_l0 = NULL;
	current_pic->ref_memory_l1 = NULL;
}

void release_pic_residu_memory(LIST_MMO* current_pic)
{
	release_memory(current_pic->residual_memory_y,current_pic->memory_size<<1);
	release_memory(current_pic->residual_memory_u,current_pic->memory_size>>1);
	release_memory(current_pic->residual_memory_v,current_pic->memory_size>>1);
	current_pic->residual_memory_y = NULL;
	current_pic->residual_memory_u = NULL;
	current_pic->residual_memory_v = NULL;
}

void release_pic_sample_memory(LIST_MMO* current_pic)
{
	release_memory(current_pic->picture_memory_y,current_pic->memory_size);
	release_memory(current_pic->picture_memory_u,current_pic->memory_size>>2);
	release_memory(current_pic->picture_memory_v,current_pic->memory_size>>2);
	current_pic->picture_memory_y = NULL;
	current_pic->picture_memory_u = NULL;
	current_pic->picture_memory_v = NULL;
}

void release_display_memory(DISPLAY_M* display)
{
	release_memory(display->display_y,display->memory_size);
	release_memory(display->display_u,display->memory_size/4);
	release_memory(display->display_v,display->memory_size/4);
	display->display_y = NULL;
	display->display_u = NULL;
	display->display_v = NULL;
}