#include "pic_error_conceal.h"
#include "symbol.h"
#include "stdlib.h"

#include "log.h"



static int get_area(int x,int y,int x1,int y1)
{
	int res = (x-x1)*(y-y1);
	return res<0?-res:res;
}

void pic_error_conceal_mp(NAL* nal,short* mv_l0_l0,short* ref_l0_l0,short* mv_l0,short* ref_l0,
						  int mb_num,int mb_width_in_mbs)
{
	int mb_idx,block_idx;
	int mb_x,mb_y;
	int block_x,block_y;
	int block_lr_x,block_lr_y;
	int mb_lr_x,mb_lr_y;
	int xintl,yintl;
	short*mb_mv_l0_l0,*mb_ref_l0_l0;
	short block_mv_x,block_mv_y,block_ref;
	int grid_x,grid_y;
	int* area;
	int area_x,area_y,area_width = mb_width_in_mbs<<2,area_num = mb_num<<4;
	int t_area,area_addr;
	int mv_addr;
	area = malloc(sizeof(int)*(mb_num<<4));
	memset(area,0,sizeof(int)*(mb_num<<4));
	memset(ref_l0,0,sizeof(short)*nal->b8_stride*mb_num/mb_width_in_mbs*4);
	for (mb_idx = 0;mb_idx<mb_num;mb_idx++)
	{
		mb_x = mb_idx%mb_width_in_mbs;
		mb_y = mb_idx/mb_width_in_mbs;
		mb_lr_x = mb_x<<4;
		mb_lr_y = mb_y<<4;
		mb_mv_l0_l0 = mv_l0_l0+((mb_y*nal->b_stride<<2)+(mb_x<<3));
		mb_ref_l0_l0 = ref_l0_l0+((mb_y*nal->b8_stride<<2)+(mb_x<<2)); 
		for (block_idx = 0;block_idx<16;block_idx++)
		{
			block_x =block_idx%4;
			block_y = block_idx/4;
			block_ref = *(mb_ref_l0_l0 +(block_y*nal->b8_stride)+block_x);
			if (block_ref < 0)
			{
				continue;
			}
			block_lr_x = mb_lr_x+(block_x<<2);
			block_lr_y = mb_lr_y+(block_y<<2);
			block_mv_x = *(mb_mv_l0_l0+(block_y*nal->b_stride)+block_x*2);
			block_mv_y = *(mb_mv_l0_l0+(block_y*nal->b_stride)+block_x*2 + 1);
			xintl = block_lr_x - (block_mv_x>>2);
			yintl = block_lr_y - (block_mv_y>>2);
			if (xintl<-4)
			{
				continue;
			}
			else if (xintl<0)
			{
				grid_x = 0;
			}
			else
			{
				grid_x = xintl|3;
				grid_x++;
			}
			if (yintl < -4)
			{
				continue;
			}
			else if (yintl < 0)
			{
				grid_y = 0;
			}
			else
			{
				grid_y = yintl|3;
				grid_y++;
			}
			
			area_x = grid_x>>2;
			area_y = grid_y>>2;
			//lu
			t_area = get_area(grid_x,grid_y,xintl,yintl);
			area_addr = (area_y-1)*area_width+area_x-1;
			mv_addr = (area_y-1)*nal->b8_stride+area_x-1;
			if (area_addr>=0 && area_addr< area_num && t_area>area[area_addr])
			{
				area[area_addr] = t_area;
				mv_l0[(mv_addr<<1)] = block_mv_x;
				mv_l0[(mv_addr<<1)+1] = block_mv_y;
			}
			//ru
			t_area = get_area(grid_x,grid_y,xintl+4,yintl);
			area_addr = (area_y-1)*area_width+area_x;
			mv_addr = (area_y-1)*nal->b8_stride+area_x;
			if (area_addr>=0 && area_addr< area_num && t_area>area[area_addr])
			{
				area[area_addr] = t_area;
				mv_l0[(mv_addr<<1)] = block_mv_x;
				mv_l0[(mv_addr<<1)+1] = block_mv_y;
			}
			//ld
			t_area = get_area(grid_x,grid_y,xintl,yintl+4);
			area_addr = (area_y)*area_width+area_x-1;
			mv_addr = (area_y)*nal->b8_stride+area_x-1;
			if (area_addr>=0 && area_addr< area_num && t_area>area[area_addr])
			{
				area[area_addr] = t_area;
				mv_l0[(mv_addr<<1)] = block_mv_x;
				mv_l0[(mv_addr<<1)+1] = block_mv_y;
			}
			//rd
			t_area = get_area(grid_x,grid_y,xintl+4,yintl+4);
			area_addr = (area_y)*area_width+area_x;
			mv_addr = (area_y)*nal->b8_stride+area_x;
			if (area_addr>=0 && area_addr< area_num && t_area>area[area_addr])
			{
				area[area_addr] = t_area;
				mv_l0[(mv_addr<<1)] = block_mv_x;
				mv_l0[(mv_addr<<1)+1] = block_mv_y;
			}
		}
	}
	//update block when area = 0
	{
		const int min_block_area = 2;
		int block_number = mb_num<<4;
		short up_x,up_y,left_x,left_y;
		for (block_idx = 0;block_idx<block_number;block_idx++)
		{
			block_x = block_idx % area_width;
			block_y = block_idx / area_width;
			mv_addr = block_y*nal->b_stride+(block_x<<1);
			if (area[block_idx] < min_block_area)
			{
				if (block_x>0)
				{
					left_x = mv_l0[mv_addr-2];
					left_y = mv_l0[mv_addr-2+1];
				}
				else
				{
					left_x = left_y = 0;
				}
				if (block_y > 0)
				{
					up_x = mv_l0[mv_addr-nal->b_stride];
					up_y = mv_l0[mv_addr-nal->b_stride+1];
				}
				else
				{
					up_x = up_y = 0;
					mv_l0[mv_addr] = left_x;
					mv_l0[mv_addr+1] = left_y;
				}
				if (0 == block_x)
				{
					mv_l0[mv_addr] = up_x;
					mv_l0[mv_addr+1] = up_y;
				}
				else
				{
					mv_l0[mv_addr] = (up_x+left_x)>>1;
					mv_l0[mv_addr+1] = (left_y+up_y)>>1;
				}
			}
		}
	}
	/*logStr("ref base error conceal result\n");
	for (block_idx = 0;block_idx<(mb_num<<4);block_idx++)
	{
		mv_addr = (block_idx/area_width)*nal->b_stride+(block_idx%area_width)<<1;
		logStr("%d %d ",mv_l0_l0[mv_addr],mv_l0_l0[mv_addr+1]);
		if (block_idx%(area_width) == area_width-1)
		{
			logStr("\n");
		}
	}
	logStr("base error conceal result\n");
	for (block_idx = 0;block_idx<(mb_num<<4);block_idx++)
	{
		mv_addr = (block_idx/area_width)*nal->b_stride+(block_idx%area_width)<<1;
		logStr("%d %d ",mv_l0[mv_addr],mv_l0[mv_addr+1]);
		if (block_idx%(area_width) == area_width-1)
		{
			logStr("\n");
		}
	}*/
	free(area);
}