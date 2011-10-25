#include "log.h"
#include "stdio.h"
#include "stdarg.h"

#include "svc_type.h"
#include "type.h"
#include "symbol.h"

FILE* logFile;
FILE* sliceLogFile;

static int nAccessUintId=0;

static char bOutputMvRef = 0;
static char bOutputMbType = 1;

void logStrSlice(const char* format,...)
{
#ifdef _LOG_
	if(sliceLogFile)
	{
		va_list arg_ptr;
		va_start(arg_ptr,format);
		vfprintf(sliceLogFile,format,arg_ptr);
		va_end(arg_ptr);
	}

#endif
	return;
}


void log_slice_info(NAL *pcNal,decoder_context* pdecoder_context,
					thread_context* pthread_context)
{
#ifdef _LOG_
	LAYER_MMO *pcLayerMmo = &(pdecoder_context->mmo->LayerMMO[pcNal->LayerId]);
	DEP_MMO* dep_mmo = pcLayerMmo->dep_mmo;
	RESIDU* current_residu = pcLayerMmo->residu;
	int i,j,k;
	SLICE* pSlice = pcNal->pSlice;
	LIST_MMO* current_pic = pcLayerMmo->currentPic;
	short* pasSliceMvl0 = current_pic->mv_memory_l0;
	short* pasSliceRefl0 = current_pic->ref_memory_l0;
	short* pasSliceMvl1 =current_pic->mv_memory_l1;
	short* pasSliceRefl1 = current_pic->ref_memory_l1;
	short sPicSizeInMbs;
	short sPicWidthInMbs;
	if (pcNal->bFinishLayerRepresent)
	{

		sPicSizeInMbs = pthread_context->current_sps_id->PicSizeInMbs;
		sPicWidthInMbs =  pthread_context->current_sps_id->pic_width_in_mbs;
		logStrSlice("*****************dqId:%d,poc:%d,nal type:%d**********\n",pcNal->DqId,
			current_pic->poc,pcNal->NalUnitType);
		logStrSlice("tcoeff:%d\n",pcNal->TCoeffPrediction);
		logStrSlice("slice type:%d\n",current_pic->slice_type);
		logStrSlice("frame_num:%d\n",pSlice->frame_num);
		logStrSlice("first_mb_num:%d\n",pSlice->first_mb_in_slice);
		logStrSlice("direct_spatial_mv:%d\n",pSlice->direct_spatial_mv_pred_flag);
		logStrSlice("baseLayerId:%d\n",pcNal->BaseLayerId);
		logStrSlice("width:%d,height:%d,bwidth:%d,bheight:%d\n",pcNal->curr_Width,pcNal->curr_Height,
			pcNal->base_Width,pcNal->base_Height);
		logStrSlice("b_stride:%d\n",pSlice->b_stride);
		logStrSlice("b8_stride:%d\n",pSlice->b8_stride);
		if (bOutputMbType)
		{
			logStrSlice("%d,mbtype ",0);
			for (j = 0;j<sPicWidthInMbs;j++)
			{
				logStrSlice("%3d ",j);
			}
			logStrSlice("\n");
			for(i = 0;i<sPicSizeInMbs/sPicWidthInMbs;i++)
			{
				logStrSlice("%d,mbtype ",i);
				for(j = 0;j<sPicWidthInMbs;j++)
				{
					RESIDU* pcMbResidu = &(current_residu[i*sPicWidthInMbs+j]);
					DATA* pcMbData = &(pthread_context->Block[i*sPicWidthInMbs+j]);
					logStrSlice("%3d ",pcMbResidu->MbType);
				}
				logStrSlice("\n");
			}
		}
		
		if(bOutputMvRef)
		{
			logStrSlice("Mv & Ref\n");
			for(i = 0;i<sPicSizeInMbs/sPicWidthInMbs;i++)
			{
				for(j = 0;j<sPicWidthInMbs;j++)
				{
					RESIDU* pcMbResidu = &(current_residu[i*sPicWidthInMbs+j]);
					if(pcMbResidu->MbType>9 && pcMbResidu->MbType<22)
					{
						logStrSlice("mbAddr:%d\n",i*sPicWidthInMbs+j);
						for(k = 0;k<16;k++)
						{
							int addr = ((i*sPicWidthInMbs+j)<<4)+k;
							logStrSlice("refl0:%d\trefl1:%d\tMvl0:%d,%d\tMvl1:%d,%d\n",pasSliceRefl0[addr],pasSliceRefl1[addr],
								pasSliceMvl0[addr<<1],pasSliceMvl0[(addr<<1)+1],pasSliceMvl1[addr<<1],pasSliceMvl1[(addr<<1)+1]);
						}
					}
					else if(pcMbResidu->MbType>4 && pcMbResidu->MbType<9)
					{
						logStrSlice("mbAddr:%d\n",i*sPicWidthInMbs+j);
						for(k = 0;k<16;k++)
						{
							int addr = ((i*sPicWidthInMbs+j)<<4)+k;
							logStrSlice("refl0:%d\tMvl0:%d,%d\n",pasSliceRefl0[addr],pasSliceMvl0[addr<<1],pasSliceMvl1[(addr<<1)+1]);
						}
					}
				}
			}
		}
				
		if (SLICE_TYPE_B == current_pic->slice_type)
		{
			logStrSlice("refPicList0 active num:%d\n",pSlice->num_RefIdxL0_active_minus1+1);
			logStrSlice("refPicList1 active num:%d\n",pSlice->num_RefIdxL1_active_minus1+1);
			for (i = 0;i<dep_mmo->RefCountL0;i++)
			{
				logStrSlice("\tpoc:%d\n",dep_mmo->refl0[i].poc);
			}
			for (i = 0;i<dep_mmo->RefCountL1;i++)
			{
				logStrSlice("\tpoc:%d\n",dep_mmo->refl1[i].poc);
			}
		}
		else if (SLICE_TYPE_P == current_pic->slice_type)
		{
			logStrSlice("refPicList0 active num:%d\n",pSlice->num_RefIdxL0_active_minus1+1);
			logStrSlice("refPicList0 real active num:%d\n",dep_mmo->RefCountL0);
			for (i = 0;i<dep_mmo->RefCountL0;i++)
			{
				logStrSlice("\tpoc:%d\n",dep_mmo->refl0[i].poc);
			}
		}
		else
		{
		}
		log_flush();
	}

#endif
}

void log_memory_state(NAL *pcNal,MMO *pcMmo)
{
#ifdef _LOG_
	int i,j;
    logStr("********************memory state*****************************\n");
    for(i = 0;i<=pcNal->LayerId;i++)
    {
        LAYER_MMO* layerMmo = &(pcMmo->LayerMMO[i]);
        DEP_MMO* dep_mmo = layerMmo->dep_mmo;
        logStr("layerId:%d\tshortRefCount:%d\tlongRefCount:%d\n",i,dep_mmo->ShortRefCount,dep_mmo->LongRefCount);
        for (j = 0;j<dep_mmo->ShortRefCount;j++)
        {
            logStr("\tpoc:%d\n",dep_mmo->ShortRef[j].poc);
        }
    }
    nAccessUintId++;
#endif
}

void log_init()
{
#ifdef _LOG_
	if(!logFile)
	{
		logFile = fopen("log.txt","w");
		sliceLogFile = fopen("sliceLog.txt","w");
	}
	else
	{
		printf("warning: has already initialized.\n");
	}
#endif
	return;
}

void log_flush()
{
#ifdef _LOG_
	if(logFile)
	{
		fflush(logFile);
		fflush(sliceLogFile);
	}
#endif
	return;
}



void logStr(const char* format,...)
{
#ifdef _LOG_
	if(logFile)
	{
		va_list arg_ptr;
		va_start(arg_ptr,format);
		vfprintf(logFile,format,arg_ptr);
		va_end(arg_ptr);
	}
	log_flush();
#endif
	return;
}


void log_exit()
{
#ifdef _LOG_
	if(logFile)
	{
		log_flush();
		fclose(logFile);
		fclose(sliceLogFile);
	}
	else
	{
		printf("warning: not open file\n");
	}
	logFile = 0;
	sliceLogFile = 0;
#endif
	return;
}



