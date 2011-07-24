#include "stdio.h"
#include "string.h"
#include "write_yuv_file.h"

FILE* yuv_file;
int init_write_yuv(char* file_name)
{
	yuv_file = fopen(file_name,"wb");
	if (0 == yuv_file)
	{
		return 0;
	}
	return 1;
}
void close_write_yuv()
{
	fclose(yuv_file);
}

void write_one_frame(unsigned char*y,unsigned char*u,unsigned char*v,int width,int height)
{
	int offset_l = (width+32)*16+16;
	int offset_c = 4*(width+32)+8;
	int i;
	y+=offset_l;
	for (i = 0;i<height;i++)
	{
		fwrite(y,1,width,yuv_file);
		y+=width+32;
	}
	width = width>>1;
	height = height>>1;
	u+=offset_c;
	for (i = 0;i<height;i++)
	{
		fwrite(u,1,width,yuv_file);
		u+=width+16;
	}
	v+=offset_c;
	for (i = 0;i<height;i++)
	{
		fwrite(v,1,width,yuv_file);
		v+=width+16;
	}
}

int parse_arg_write_yuv(int argc,char* argv[],char* fileName)
{
	int i =0;
	for (i = 1;i<argc;i++)
	{
		if (0 == strcmp(argv[i],"-write_yuv"))
		{
			if (i<argc-1)
			{
				strcpy(fileName,argv[i+1]);
				return 1;
			}
			printf("not set yuv file name\n");
			return 0;
		}
	}
	return 0;
}