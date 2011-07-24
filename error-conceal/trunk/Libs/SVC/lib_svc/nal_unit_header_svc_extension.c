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

#include <string.h> 


//AVC Files
#include "type.h"
#include "nal.h"
#include "bitstream.h"
#include "slice_data_vlc.h"


//SVC Files
#include "svc_type.h"
#include "set_layer.h"
#include "SwitchAdress.h"
#include "pic_list_svc.h"
#include "LayerManagement.h"
#include "decoder_context.h"
#include "nal_unit_header_svc_extension.h"
#include "layer_utils.h"

#include "assert.h"

extern int	RemainBytes ;

/**
This function permits to get the length of the next NAL.
This function search from the data read from the bistream the next start_code.

@param data The buffer read from the bistream.
@param rbsp_byte Buffer containing the data to decode.
@param NumBytesInRBSP Size of the NAL to decode.
*/
 int GetNalBytesInNalunit ( unsigned char *data, NAL * current_nal, int *NumBytesInRBSP, ConfigSVC *StreamType)
{

    int i;
	int offset;
	int nal_unit_type;
	unsigned char *NextNal;
	int NBInRBSP = 0;
    int NalInRBSP = search_start_code(data, &offset);
	unsigned char* rbsp_memory;

	if (current_nal->nrbsp_memory_size < NalInRBSP+1)
	{
		if (current_nal->prbsp_memory)
		{
			free(current_nal->prbsp_memory);
		}
		current_nal->nrbsp_memory_size = NalInRBSP+1;
		current_nal->prbsp_memory = (unsigned char*)malloc(NalInRBSP+1);
	}
	rbsp_memory = current_nal->prbsp_memory;
	for ( i = offset ; i < NalInRBSP + offset; i++ ) {
        rbsp_memory [NBInRBSP++] = data [i];
		if ( i + 2 < (NalInRBSP + offset) && show3Bytes_Nal(data, i) == 3 ) {
            rbsp_memory [NBInRBSP++] = data [i++] ;
            i ++ ;
		}
    }

	rbsp_memory [NBInRBSP] = data [i];
	*NumBytesInRBSP =  NalInRBSP + offset;

	//Get the next Nal to know its type
	if(*NumBytesInRBSP < RemainBytes){
		NalInRBSP = 0;

		//determination of the length of the start code ( 3 or 4 )
		for (offset = 0 ; NalInRBSP != 1 ; offset++ ) 
			NalInRBSP = show3Bytes_Nal(data, offset);

		offset += 2;

		//Get the next nal unit type
		NextNal = &data[i + offset];
		nal_unit_type = NextNal[0] & 0x1F;

		//Get the dqid if it's a SVC nal
		if (StreamType)
		{
			if ( nal_unit_type == 14 || nal_unit_type == 20){
				int Position = 8;
				int ReservedOneBits = getNbits(NextNal, &Position, 9) ;// u(1)  reserved_one_bits =
				int DependencyId = getNbits(NextNal, &Position, 3) ;// u(3)
				int QualityId = getNbits(NextNal, &Position, 4) ;// u(4) quality_id = 
				StreamType -> NextDqId = (DependencyId << 4 ) + QualityId;
			}
			else
			{
				StreamType->NextDqId = -1;
			}
		}
	}
	else
	{
		StreamType->NextDqId = -1;
	}
		

	
	return NBInRBSP ;
}





/**
This function allows to initialize the NAL structure.

*/




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
This function allows to initialize the do_dec_ref_pic_marking_base variable.
If the current nal can be skipped, it's equal to 3
Else to zero.
*/
void SetDefRecPic(NAL *Nal,unsigned char *do_dec_ref_pic_marking_base)
{
	if( (Nal -> DqId < Nal -> DqIdToDisplay && Nal -> DiscardableFlag) || (Nal -> DqId > Nal -> DqIdToDisplay) ){
		//DqId superior to the layer wanted, or inferior but can be discarded.
		//The layer is discarded, only its poc is calculated
		*do_dec_ref_pic_marking_base = 3;	
		Nal -> PicToDisplay = 0;	//Reset parameters
		Nal -> NalToSkip = 1;		//Reset parameters
	}
	else {
		*do_dec_ref_pic_marking_base = 0;	
	}
}


/**
This functions sets to actived or desactived NAL decoding process.
*/
void SetPicToDisplay(NAL *Nal)
{
	int MaxTemporalId = GetSeqTempId();

	
	if (Nal -> TemporalId <= MaxTemporalId){
		//We want to display this layer
		Nal -> NalToSkip = 0;
	}else{
		Nal -> NalToSkip = 1;
	}


	if( (Nal -> DqId < Nal -> DqIdToDisplay && Nal -> DiscardableFlag) || (Nal -> DqId > Nal -> DqIdToDisplay) ){
		//DqId superior to the layer wanted, or inferior but can be discarded.
		//The layer is discarded, only its poc is calculated
		Nal -> PicToDisplay = 0;	//Reset parameters
		Nal -> NalToSkip = 1;		//Reset parameters
	}else if (Nal -> DqId < Nal -> DqIdToDisplay){
		Nal -> PicToDisplay = 0;	//Reset parameters
	}else{
		Nal -> PicToDisplay = !Nal -> NalToSkip; //Display depends on the anl decoding
	}
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

	//Displayed layer selection
	SetDefRecPic(Nal, DecRefPic);
		

	//Assume that prefix indicates DqId = 0
	if (Nal->layer_id_table[0] == -1)
	{
		add_layer_id(0,0);
	}
	Nal->LayerId = 0;

	if( Nal -> NalRefIdc != 0 ) {
		Nal -> StoreRefBasePicFlag = getNbits(Data, &position, 1) ;
		if( ( Nal -> UseRefBasePicFlag || Nal -> StoreRefBasePicFlag ) && !Nal -> IdrFlag ){
			DecRefBasePicMarking(Data, &position, Mmo, Sps, Nal);
		}

		if( getNbits(Data, &position, 1) == 1 ){
			while( more_rbsp_data(Data, &position, NalInRbsp ) ){
				getNbits(Data, &position, 1);//prefix_nal_unit_extension_flag 2 u(1)
			}
		}
	}
	Nal->NextDqId = Nal->DqId;
}








/**
This function is used to know the layer's caracteristic as the layer ID, the temporal ID ...

@param data The NAL unit.
@param NalInRbsp NAL unit's Size.
@param
*/
void NalUnitHeader (unsigned char *Data, int *position, unsigned char *do_dec_ref_pic_marking_base, NAL *Nal, unsigned char *EndOfSlice)
{	

	int i;


	*position = 8;
	Nal -> LastLayerId = Nal -> LayerId;
	Nal -> LastDqId = Nal -> DqId;
	Nal -> LastTemporalId = Nal -> TemporalId;

	//Get NAL header information
	NalUnitData(Data, position, Nal, EndOfSlice);


	for ( i = 1; i < 8; i++){
		if ( Nal -> layer_id_table[i] == -1){
			add_layer_id(Nal->DqId,i);
			Nal -> LayerId = i;
			break;
		}
		if(Nal -> layer_id_table[i] == Nal -> DqId){
			Nal -> LayerId = i;
			break;
		}
		if(Nal -> layer_id_table[i] > Nal -> DqId){
			assert(0);
			insert_layer_id(Nal->DqId,i);
			Nal->LayerId = i;
			break;
		}
	}

	
	SetDefRecPic(Nal, do_dec_ref_pic_marking_base);
}












/**
This function determines if the current layer has to be fully or partially decoded 
Nal -> PicToDisplay == 1 => Nal -> PicToDecode == 1  ==> fully decoded
Nal -> PicToDisplay == 0 and CUrrent layer needed for upper layer => Nal -> PicToDecode == 1  ==> partially decoded
*/
void IsPicToDecode(NAL *Nal){
	// if we want to display this layer, or we need for spatial enhencement.
	int WantedLayerNumber = GetLayerNumber (Nal, Nal -> DqIdToDisplay);
	Nal -> PicToDecode = Nal -> PicToDisplay || (Nal -> UsedSpatial [Nal -> LayerId] [WantedLayerNumber] > 0);
}



/**
This function determines if the current layer has to be decoded entirely, or not.
This function is used in case of the NAL has to be decoded.
*/
void CheckPicToDisplay(NAL *Nal, int EndOfSlice,decoder_context* pdecoder_context){

	if (!EndOfSlice){
		//Make sure that there is no slice.
		if (!Nal -> NalToSkip && (Nal -> DqIdToDisplay == Nal -> DqId)){
			// Wanted DqId or we have nothing else to decode.
			Nal -> PicToDisplay = 1;
		}else {
			Nal -> PicToDisplay = 0;
		}
		//To decode base layer info or not
		IsPicToDecode(Nal);
	}
}





/**
This function determines if the current layer has to be decoded entirely, or not.
This function is used in case of the NAL has to be decoded.
only for upper layer which are skipped
*/
void CheckSkipPicToDisplay(NAL *Nal, int EndOfSlice,decoder_context *pdecoder_context){
	
	if (!EndOfSlice){
		//Make sure that there is no slice.
		if (Nal -> NextDqId <= Nal -> DqId){
			// Detect missing slice
			Nal -> PicToDisplay = 1;
			//To make sure, in case of missing layer, to display something
			if(Nal -> NextDqId <= Nal -> DqId){
				pdecoder_context->DqIdMax = Nal -> DqId;
			}
		}else {
			Nal -> PicToDisplay = 0;
		}
		//To decode base layer info or not
		IsPicToDecode(Nal);
	}
}








/**
This function determines if the current layer has to be decoded entirely, or not.
This function is used in case of the NAL has to be decoded.
*/
void SetBaseNalToDisplay(NAL *Nal, int EndOfSlice,decoder_context* pdecoder_context){

	if (!EndOfSlice && !Nal -> NalToSkip){
		//Make sure that there is no slice.
		if ( (Nal -> DqIdToDisplay == Nal -> DqId)){
			// Wanted DqId or we have nothing else to decode.
			Nal -> PicToDisplay = 1;
			//To make sure, in case of missing layer, to display something
			if(Nal->bFinishAccessUnit){
				pdecoder_context -> DqIdMax = Nal -> DqId;
			}
		}else {
			Nal -> PicToDisplay = 0;
		}
	}
}


void NalUnitLayerInfo(NAL* nal)
{
	if (!((nal->NalUnitType>=1 && nal->NalUnitType<=5)||nal->NalUnitType == 20))
	{
		nal->bFinishAccessUnit = 0;
		nal->bFinishDependenceRepresent = 0;
		nal->bFinishLayerRepresent = 0;
	}
	else if (-1 == nal->NextDqId ||
		(nal->NextDqId == nal->DqId && 0 == nal->DqId)||
		nal->NextDqId<nal->DqId)
	{
		nal->bFinishDependenceRepresent = 1;
		nal->bFinishLayerRepresent = 1;
		nal->bFinishAccessUnit = 1;
	}
	else if (nal->NextDqId >>4 != nal->DqId>>4)
	{
		nal->bFinishLayerRepresent = 1;
		nal->bFinishDependenceRepresent = 1;
		nal->bFinishAccessUnit = 0;
	}
	else if(nal->DqId != nal->NextDqId)
	{
		nal->bFinishAccessUnit = 0;
		nal->bFinishDependenceRepresent = 0;
		nal->bFinishLayerRepresent = 1;
		
	}
	else
	{
		nal->bFinishLayerRepresent = 0;
		nal->bFinishDependenceRepresent = 0;
		nal->bFinishAccessUnit = 0;
	}
}