#ifndef _WRITE_YUV_FILE_H_
#define _WRITE_YUV_FILE_H_

int init_write_yuv(char* file_name);
void close_write_yuv();


void write_one_frame(unsigned char*y,unsigned char*u,unsigned char*v,int width,int height);

int parse_arg_write_yuv(int argc,char* argv[],char* fileName);

#endif