#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "type.h"

void* allocate_memory(int size);
void release_memory(void* p,int size);

void allocate_picture_memory(LIST_MMO* current_pic);
void allocate_mv_memory(LIST_MMO* current_pic);
void release_picture_memory(LIST_MMO* current_pic);
void release_pic_mv_memory(LIST_MMO* current_pic);
void release_pic_residu_memory(LIST_MMO* current_pic);
void release_pic_sample_memory(LIST_MMO* current_pic);
void release_pic_pic_memory(LIST_MMO* current_pic);
void allocate_pic_sample_memory(LIST_MMO* current_pic);

void release_display_memory(DISPLAY_M* display);

void init_memory_cache();
void free_memory_cache();
#endif