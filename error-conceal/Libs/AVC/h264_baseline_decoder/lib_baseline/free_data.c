#include "free_data.h"
#include "stdlib.h"

void free_mmo(MMO *mmo)
{
	int i = 0;
	for (i = 0;i<8;i++)
	{
		LAYER_MMO * layerMMO = &(mmo->LayerMMO[i]);
		if (layerMMO->mb_slice_group_map)
		{
			free(layerMMO->mb_slice_group_map);
			free(layerMMO->mb_slice_table);
		}
	}
}


void free_nal(NAL* pNal)
{
	if(pNal)
	{
		free(pNal->prbsp_memory);
		free(pNal);
	}
	
}