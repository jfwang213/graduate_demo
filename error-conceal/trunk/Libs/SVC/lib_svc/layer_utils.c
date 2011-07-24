#include "layer_utils.h"
#include "type.h"
#include "decoder_context.h"

int get_layer_id(char dqId,char* layer_id_table)
{
	int i;
	for (i = 0;i<8;i++)
	{
		if (layer_id_table[i] == dqId)
		{
			return i;
		}
		else if (layer_id_table[i] == -1)
		{
			return -1;
		}
	}
	return -1;
}

int get_same_dep_highest_layer_id(int layer_id,char* layer_id_table)
{
	int res;
	int depId = layer_id_table[layer_id]>>4;
	if (-1 == layer_id_table[layer_id])
	{
		return -1;
	}
	
	for (res = layer_id+1;res<8;res++)
	{
		if (layer_id_table[res]>>4 != depId)
		{
			break;
		}
	}
	return res-1;
}
extern decoder_context pdecoder_context[1];
void add_layer_id(char dqId,int layer_id)
{
	int depId = dqId>>4;
	MMO*	pMmo = pdecoder_context->mmo;
	LAYER_MMO* pLayerMmo = &(pMmo->LayerMMO[layer_id]);
	pdecoder_context->layer_id_table[layer_id] = dqId;
	pLayerMmo->dep_mmo = &(pMmo->DepMMO[depId]);
}

void insert_layer_id(char dqId,int layer_id)
{
	MMO*	pMmo = pdecoder_context->mmo;
	int i;
	for (i = NUM_LAYERS-1;i>layer_id;i--)
	{
		pMmo->LayerMMO[i] = pMmo->LayerMMO[i-1];
		pdecoder_context->layer_id_table[i] = pdecoder_context->layer_id_table[i-1];
	}
	add_layer_id(dqId,layer_id);
}