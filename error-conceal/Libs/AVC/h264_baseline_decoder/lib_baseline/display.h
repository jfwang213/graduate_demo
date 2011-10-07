#ifndef _DISPLAY_H_
#define _DISPLAY_H_


void FinishFrameSVC(const int NbMb,NAL *Nal,SPS *Sps,PPS *Pps,LIST_MMO *Current_pic,
					SLICE *Slice,int EndOfSlice,unsigned char *SliceTab,DATA *TabBlbock,
					RESIDU *Residu,short *MvL0,short *MvL1,short *RefL0,short *RefL1,
					int *Crop,int *ImgToDisplay,DISPLAY_M *display_memory,MMO *Mmo,
					unsigned char *RefY,unsigned char *RefU,unsigned char *RefV,
					int *xsize,int *ysize);
void DisplayOrderSvc(SPS *sps,LIST_MMO *current_pic,NAL *nal,DISPLAY_M *display_memory,,
					 int *x_size,int *y_size,int *Crop,int *img_to_display,MMO *mmo);

#endif

