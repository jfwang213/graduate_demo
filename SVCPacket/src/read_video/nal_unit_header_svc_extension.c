#include <string.h> 


//AVC Files
#include "type.h"
#include "nal.h"
#include "bitstream.h"


//SVC Files
#include "svc_type.h"
#include "decoder_context.h"
#include "nal_unit_header_svc_extension.h"


/**
This function is sure to determine the Nal unit type of the current nal and to initilize the idr flag in case of AVC NAL.
*/
void NalUnitSVC(unsigned char *data_in,  NAL *Nal)
{

	//Read from the buffer the NAL unit type
	int tmp = data_in[0];
	Nal -> NalRefIdc = tmp >> 5 ;
	Nal -> NalUnitType = tmp & 0x1F ;

	//To avoid AVC stream crash 
	Nal -> no_inter_layer_pred_flag = 1;
	if ( Nal -> NalUnitType == 5){
		Nal -> IdrFlag = 1;
	}else {
		Nal -> IdrFlag = 0;
	}
	//Error detection
	//Due when NalUnitType == 14 lost.
	if(Nal -> NalUnitType < 6){
		Nal -> LayerId = 0;
	}
	Nal->current_position[0] = 8;
}



/**
Parses the NAL unit header.
*/
void NalUnitData(unsigned char *Data, int *position, NAL *Nal, unsigned char *EndOfSlice)
{

	getNbits(Data, position, 1) ;// u(1)  reserved_one_bits =
	Nal -> IdrFlag = getNbits(Data, position, 1) ;// u(1)  idr_flag =
	getNbits(Data, position, 6) ;// u(6)  priority_id = 
	Nal -> no_inter_layer_pred_flag = getNbits(Data, position, 1) ;// u(1) no_inter_layer_pred_flag
	Nal -> DependencyId = getNbits(Data, position, 3) ;// u(3)
	Nal -> QualityId = getNbits(Data, position, 4) ;// u(4) quality_id = 
	Nal -> DqId = ( Nal -> DependencyId << 4 ) + Nal -> QualityId;
	Nal -> TemporalId = getNbits(Data, position, 3) ;// u(3) temporal_id = 
	Nal -> UseRefBasePicFlag = getNbits(Data, position, 1) ;// u(1)
	Nal -> DiscardableFlag = getNbits(Data, position, 1) ;// u(1)   discardable_flag = 
	getNbits(Data, position, 1) ;//output_flag All u(1)
	getNbits(Data, position, 2) ;//reserved_three_2bits


	

	Nal -> ErrorDetected = 0;
	if(Nal -> LastDqId != Nal -> DqId && *EndOfSlice){
		//*EndOfSlice = 0;
		Nal -> ErrorDetected = 1;
	}
}



/** 
This function is used before an AVC NAL, in order to describe to the decoder the Nal caracteristic.

*/
void PrefixNalUnit(unsigned char *Data, int NalInRbsp, NAL *Nal, MMO *Mmo, SPS *Sps, 
				   unsigned char *DecRefPic, unsigned char *EndOfSlice,decoder_context* pdecoder_context)
{

	int position = 8;
	//Get NAL header information
	NalUnitData(Data, &position, Nal, EndOfSlice);
	//Initialize the dependency Max if necessary
	if (pdecoder_context -> DqIdMax < Nal -> DqId){
		pdecoder_context -> DqIdMax = Nal -> DqId;
	}

	//Initialize the dependency Max if necessary
	if (pdecoder_context -> DependencyIdMax < Nal -> DependencyId){
		pdecoder_context -> DependencyIdMax = Nal -> DependencyId;
	}



	//Initialize the Temporal Id Max if necessary
	if (pdecoder_context -> MaxTemporalId <= Nal -> TemporalId){
		pdecoder_context -> MaxTemporalId = Nal -> TemporalId;
	}
}








/**
This function is used to know the layer's caracteristic as the layer ID, the temporal ID ...

@param data The NAL unit.
@param NalInRbsp NAL unit's Size.
@param
*/
void NalUnitHeader (unsigned char *Data, int *position, unsigned char *do_dec_ref_pic_marking_base, NAL *Nal, unsigned char *EndOfSlice)
{	

	int i, j;


	*position = 8;
	Nal -> LastLayerId = Nal -> LayerId;
	Nal -> LastDqId = Nal -> DqId;
	Nal -> LastTemporalId = Nal -> TemporalId;

	//Get NAL header information
	NalUnitData(Data, position, Nal, EndOfSlice);


	for ( i = 1; i < 8; i++){
		if ( Nal -> layer_id_table[i] == -1){
			Nal -> layer_id_table[i] = Nal -> DqId;
			Nal -> LayerId = i;
			break;
		}
		if(Nal -> layer_id_table[i] == Nal -> DqId){
			Nal -> LayerId = i;
			break;
		}
		if(Nal -> layer_id_table[i] > Nal -> DqId){
			Nal -> LayerId = i;
			for ( j = i; j < 7; j ++){
				Nal -> layer_id_table[i + 1] =  Nal -> layer_id_table[i];
			}
			Nal -> layer_id_table[i] = Nal -> DqId;
			break;
		}
	}

	
}








