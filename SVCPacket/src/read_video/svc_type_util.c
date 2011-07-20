
#include "decoder_context.h"
#include "svc_type_util.h"
#include "stdlib.h"
char is_vcl_nal(NAL* pNal)
{
	if ((1 <= pNal->NalUnitType && 
		pNal->NalUnitType<=5)||(20 == pNal->NalUnitType))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void copy_nal_from_prefix(NAL *prefix_nal,NAL *current_nal)
{

	current_nal->LayerId = prefix_nal->LayerId;
	current_nal->IdrFlag = prefix_nal->IdrFlag;
	current_nal->no_inter_layer_pred_flag = prefix_nal->no_inter_layer_pred_flag;
	current_nal->DependencyId = prefix_nal->DependencyId;
	current_nal->QualityId = prefix_nal->QualityId;
	current_nal->DqId = prefix_nal->DqId;
	current_nal->TemporalId = prefix_nal->TemporalId;
	current_nal->UseRefBasePicFlag = prefix_nal->UseRefBasePicFlag;
	current_nal->DiscardableFlag = prefix_nal->DiscardableFlag;

	current_nal->StoreRefBasePicFlag = prefix_nal->StoreRefBasePicFlag;
}

void init_nal(NAL* pNal,decoder_context* pdecoder_context)
{
	//init slice here
	//picture is not init here because maybe some nal share a picture
	int i;
	pNal->pSlice = (SLICE*)malloc(sizeof(SLICE));
	pNal->pic_parameter_id = pdecoder_context->pic_parameter_id;
	for (i = 0;i<8;i++)
	{
		pNal->UsedSpatial[i] = pdecoder_context->UsedSpatial[i];
	}
	pNal->layer_id_table = pdecoder_context->layer_id_table;
	pNal->current_position[0] = 0;
	pNal->DqIdToDisplay = pdecoder_context->dq_to_display;
}

void free_nal(NAL* pNal)
{
	free(pNal->pSlice);
	free(pNal);
}