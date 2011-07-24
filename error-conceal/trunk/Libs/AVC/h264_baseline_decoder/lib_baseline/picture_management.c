/*****************************************************************************
   *
   *  Open SVC Decoder developped in IETR image lab
   *
   *
   *
   *              Médéric BLESTEL <mblestel@insa-rennes.Fr>
   *              Mickael RAULET <mraulet@insa-rennes.Fr>
   *              http://www.ietr.org/
   *
   *
   *
   *
   *
   * This library is free software; you can redistribute it and/or
   * modify it under the terms of the GNU Lesser General Public
   * License as published by the Free Software Foundation; either
   * version 2 of the License, or (at your option) any later version.
   *
   * This library is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   * Lesser General Public License for more details.
   *
   * You should have received a copy of the GNU Lesser General Public
   * License along with this library; if not, write to the Free Software
   * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
   *
   *
   * $Id$
   *
   **************************************************************************/



#include "type.h"
#include "data.h"
#include "picture_management.h"
#include "memory.h"

extern void logStr(const char* format,...);
/**
This function permits to remove the oldest image in the reference list.

@param mmo MMO structure which contains all information about memory management of the video.
*/
void sliding_window(MMO *mmo, DEP_MMO *depMmo){

	int pos_best = depMmo -> ShortRefCount - 1;
	//Avoid pb when bits stream error
	if(pos_best >= 0){
		remove_short(mmo, depMmo, depMmo -> ShortRef[ pos_best ].pic_num,depMmo -> ShortRef[ pos_best ].bBasePic);
	}
}



/**
This function permits to remove the unused image in the short and long reference list.

@param mmo MMO structure which contains all information about memory management of the video.
*/
void remove_unused(MMO *mmo, DEP_MMO *depMmo){
	int i;
	int removed_flag = 0;

	for(i = 0 ; i < depMmo -> ShortRefCount ; i++){
		if ( depMmo -> ShortRef[i] . reference == 0 ){
			remove_short(mmo,  depMmo, depMmo -> ShortRef[i].pic_num,depMmo->ShortRef[i].bBasePic);
			removed_flag = 1;
			break;
		}
	}
	if (!removed_flag){
		sliding_window(mmo, depMmo);
	}
}





/**
This function permits to remove an image in the short reference list.

@param mmo MMO structure which contains all information about memory management of the video.
@param frame_num The number of the frame to eliminate.
*/
void remove_short(MMO *mmo, DEP_MMO *depMmo, int pic_num,char bBasePic){

    int i;
	LIST_MMO* pic;
	LAYER_MMO* pcMemoryLayerMmo;
	if ( depMmo -> ShortRefCount){
		for(i = 0; i < depMmo -> ShortRefCount; i++){
			pic = &(depMmo -> ShortRef[i]);
			if(pic->pic_num == pic_num && pic->bBasePic == bBasePic){
				logStr("remove short\tlayerId:%d\tpoc:%d\n",pic->ucLayerId,pic->poc);
				pcMemoryLayerMmo = &(mmo->LayerMMO[pic->ucLayerId]);
				if ( pic->displayed == 1){
					release_pic_sample_memory(pic);
					logStr("relase pic layerId:%d\tpoc:%d\n",pic->ucLayerId,pic->poc);
				}
				release_pic_mv_memory(pic);
				logStr("release mv layerId:%d\tpoc:%d\n",pic->ucLayerId,pic->poc);
				for (; i < depMmo -> ShortRefCount; i++){
					depMmo -> ShortRef[i] = depMmo -> ShortRef[i + 1];
				}
				depMmo -> ShortRefCount--;
				break;
			}
		}
		return;
	}
}




/**
This function permits to transform an short reference image in a long one.

@param mmo MMO structure which contains all information about memory management of the video.
@param frame_num The number of the frame to covert.
@param Long_Term_Idx Index to set to the image to convert.
*/
void short_to_long(MMO *Mmo, DEP_MMO *depMmo, int pic_num, int Long_Term_Idx,char bBasePic){

	int i;
	LIST_MMO pic;

	LIST_MMO* longRef;
	char* longRefCount;
	if (bBasePic)
	{
		longRef = depMmo->LongRefBase;
		longRefCount = &(depMmo->LongRefCountBase);
	}
	else
	{
		longRef = depMmo->LongRef;
		longRefCount = &(depMmo->LongRefCount);
	}
	if (depMmo -> ShortRefCount){
		for(i = 0; i < depMmo -> ShortRefCount; i++){
			pic = depMmo -> ShortRef[i];
			if(pic . pic_num == pic_num && pic.bBasePic == bBasePic){


				longRef [Long_Term_Idx] = pic;
				longRef[Long_Term_Idx] . pic_id = Long_Term_Idx;
				longRef[Long_Term_Idx].long_term = 1;
				(*longRefCount)++;

				//remove short
				for (; i < depMmo -> ShortRefCount; i++){
					depMmo -> ShortRef[i] = depMmo -> ShortRef[i + 1];
				}
				depMmo->ShortRefCount--;
				break;
			}
		}
	}
}





/**
This function permits to remove an image in the long reference list.

@param mmo MMO structure which contains all information about memory management of the video.
@param long_term_pic_num The picture number of the frame to eliminate.
*/
void remove_long(MMO *mmo, DEP_MMO *depMmo, int long_term_pic_num,char bBaseLayer){


	LIST_MMO* pcLongRef;
	char* pnLongRefCount;
	if (bBaseLayer)
	{
		pcLongRef = depMmo->LongRefBase;
		pnLongRefCount = &(depMmo->LongRefCountBase);
	}
	else
	{
		pcLongRef = depMmo->LongRef;
		pnLongRefCount = &(depMmo->LongRefCount);
	}
	if (pcLongRef[long_term_pic_num].reference == 3){
		if (pcLongRef[long_term_pic_num] . displayed == 1){
			release_pic_sample_memory(pcLongRef);
		}
		logStr("remove long\tlayerId:%d\n",pcLongRef[long_term_pic_num].ucLayerId);
		release_pic_mv_memory(pcLongRef);
		pcLongRef[long_term_pic_num] . reference = 0;
		(*pnLongRefCount)--;
	}
}




/**
This function permits to remove all image.

@param mmo MMO structure which contains all information about memory management of the video.
*/
void remove_all(MMO *mmo, DEP_MMO *depMmo){
	int i;
	int count = depMmo -> ShortRefCount;
	
	for(i = 0; i < count; i++){
		if(1 == depMmo->ShortRef[i].displayed){
			//mmo -> FreeMemoryAddress[mmo -> NumFreeAddress++] = LayerMmo -> ShortRef[i] . MemoryAddress;
			release_pic_sample_memory(&(depMmo->ShortRef[i]));
		}
		logStr("remove short\tlayerId:%d\n",depMmo->ShortRef[i].ucLayerId);
		release_pic_mv_memory(&(depMmo->ShortRef[i]));
	}
	depMmo->ShortRefCount = 0;
	for (i = 0 ; i < 16;  i++){
		if (depMmo -> LongRef[i] . reference == 3){
			if(depMmo -> LongRef[i] . displayed == 1){
				
				release_pic_sample_memory(&(depMmo -> LongRef[i]));
			}
			logStr("remove long\tlayerId:%d\n",depMmo -> LongRef[i].ucLayerId);
			release_pic_mv_memory(&(depMmo -> LongRef[i]));
			
			depMmo -> LongRef[i] . reference = 0;
		}
	}
	for (i = 0 ; i < 16;  i++){
		if (depMmo -> LongRefBase[i] . reference == 3){
			if(depMmo -> LongRefBase[i] . displayed == 1){
				
				release_pic_sample_memory(&(depMmo -> LongRefBase[i]));
			}
			logStr("remove base long\tlayerId:%d\n",depMmo -> LongRefBase[i].ucLayerId);
			release_pic_mv_memory(&(depMmo -> LongRefBase[i]));

			depMmo -> LongRefBase[i] . reference = 0;
		}
	}
	depMmo -> LongRefCount = 0;
	depMmo->LongRefCountBase = 0;
}
