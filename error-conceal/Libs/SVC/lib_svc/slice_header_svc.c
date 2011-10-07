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

//AVC Files
#include "type.h"
#include "data.h"
#include "bitstream.h"
#include "cavlc.h"
#include "slice_header.h"
#include "quantization.h"
#include "memory_management.h"
#include "memory.h"
#include "stdio.h"


//SVC Files
#include "svc_type.h"
#include "svc_data.h"
#include "set_layer.h"
#include "SwitchAdress.h"
#include "pic_list_svc.h"
#include "ErrorDetection.h"
#include "slice_data_I_svc.h"
#include "SliceHeaderExtension.h"
#include "nal_unit_header_svc_extension.h"

#include "log.h"
#include "ErrorConcealment.h"


#ifdef ENABLE_PACKET_LOSS_DETECTION


void PacketDropInSlice (SLICE *Slice, int *end_of_slice){
	// Packet drop detection algorithm
	// NOTE: this works with similarly sliced frame.
	// In the beginning there shouldn't be error until the cycle is ready

	if(!Slice -> slice_cycle_ready)
	{
		int i, first, tmp;

		// Set value to table
		Slice -> first_mb[Slice -> first_mb_index] = Slice -> first_mb_in_slice;
		
		// Check whether the cycle is ready
		if(Slice -> first_mb_index > 0)
		{
			first = Slice -> first_mb[0];
			for(i = 1; i < (Slice -> first_mb_index+1); i++)
			{
				tmp = Slice -> first_mb[i];
				// Slice cycle found
				if(tmp == first){
					Slice -> slice_cycle_ready = 1;
					Slice -> num_of_slices_per_frame = i;
					break;
				}
			}
		}

		Slice -> first_mb_index++;

		// Start index from the beginning if cycle is ready
		if(	Slice -> slice_cycle_ready ) {
			Slice -> first_mb_index = 0;
		}

		// for sanity check
		if(Slice -> first_mb_index == MAX_SLICES_PER_FRAME)
		{
			Slice -> slice_cycle_ready = 1; // table is full and no cycle found. packet loss detection not possible
		}
	}
	
	if (Slice -> slice_cycle_ready &&
		(Slice -> num_of_slices_per_frame != 0)) {
		
		if(Slice -> first_mb_index >= (Slice -> num_of_slices_per_frame)) {
			Slice -> first_mb_index = 0;
		}
		Slice -> first_mb_in_slice_expected = Slice -> first_mb[Slice -> first_mb_index];
		
		// Check whether expexted slice is the same than received
		if(Slice -> first_mb_in_slice_expected != Slice -> first_mb_in_slice)
		{
			//printf("err Slice -> first_mb_in_slice %i\n", Slice -> first_mb_in_slice);
			printf("err Slice -> first_mb_in_slice_expected %i\n", Slice -> first_mb_in_slice_expected);
			
			// increase index because of lost packet 
			Slice -> first_mb_index++;

			// Packet loss found. Do packet loss error handling
			*end_of_slice = 1;
		}
		Slice -> first_mb_index++;
	}
}

#endif





//Slice header for SVC extension

int slice_header_svc(unsigned char *data, SPS *sps_id, PPS *pps_id,
					  unsigned char *entropy_coding_flag, W_TABLES *quantif, LIST_MMO *Current_pic, 
					  SPS *sps, PPS *pps, int *position, SLICE *Slice, MMO *Mmo, 
					  LIST_MMO RefPicListL0[], LIST_MMO RefPicListL1[], NAL *Nal, unsigned char *end_of_slice,int dq_id_display,
					  decoder_context* pdecoder_context,thread_context* pthread_context) 
{


	//This test allows to avoid crashing when 
	//Sps and IDR frame are missing
	if(Mmo -> MemoryAllocation){
		PPS *pt_pps_id ;
		SPS *pt_sps_id ;	
		int PicParameterId;
		int SeqParameterId;
		LAYER_MMO *LayerMmo = &Mmo -> LayerMMO [Nal -> LayerId];
		DEP_MMO* dep_mmo = LayerMmo->dep_mmo;
		
		int LastFrameNum = dep_mmo->prev_frame_num;
		int PrevEndOfSlice = *end_of_slice;
		int FirstMbInSlice = Slice -> first_mb_in_slice;
		
		//
		//init dep information
		LayerMmo->receive_new_nal = 1;
		dep_mmo->do_ref_pic_mark = 0;
		dep_mmo->receive_one_layer = 1;
		dep_mmo->active = 1;
		//Read header data
		Slice -> first_mb_in_slice = read_ue(data, position);
		Current_pic -> slice_type = Slice -> slice_type = read_ue(data, position);
		if ( Slice -> slice_type > 4 ) {
			Current_pic -> slice_type = Slice -> slice_type -= 5 ;
		}
		PicParameterId = read_ue(data, position);
		Nal -> pic_parameter_id[Nal -> LayerId] = PicParameterId = CLIP3(0, PPS_BUF_SIZE - 1, PicParameterId);
		pt_pps_id = &pps[PicParameterId];
		//Should be in slice header
		//Sometimes Subset SPS are after PPS (rtsp)
		if(pt_pps_id -> seq_parameter_set_id > sps[0] . MaxSpsId){
			pt_pps_id -> seq_parameter_set_id = sps[0] . MaxSpsId;
		}

		SeqParameterId = pt_pps_id -> seq_parameter_set_id + (Nal -> LayerId ? 16: 0);
		pt_sps_id = &sps[SeqParameterId];
		Slice -> frame_num = getNbits(data, position, pt_sps_id -> log2_max_frame_num );

		//check PicSizeInMap  memory
		if(LayerMmo->PicSizeInMapUnits < pt_sps_id->pic_width_in_mbs*pt_sps_id->pic_height_in_map_units)
		{
			LayerMmo->PicSizeInMapUnits = pt_sps_id->pic_width_in_mbs;
			LayerMmo->PicSizeInMapUnits *= pt_sps_id->pic_height_in_map_units;
			free(LayerMmo->mb_slice_group_map);
			free(LayerMmo->mb_slice_table);
			LayerMmo->mb_slice_group_map = (unsigned char*)malloc(LayerMmo->PicSizeInMapUnits);
			LayerMmo->mb_slice_table = (unsigned char*)malloc(LayerMmo->PicSizeInMapUnits);
		}
		//cal frame_num_offset
		if (Nal->IdrFlag)
		{
			Slice->frame_num_offset = 0;
		}
		else
		{
			if (dep_mmo->prev_frame_num > Slice->frame_num)
			{
				Slice->frame_num_offset = dep_mmo->prev_frame_num+sps_id->log2_max_frame_num;
			}
		}
		*entropy_coding_flag = pt_pps_id -> entropy_coding_mode_flag;

		//EndOfSlice is set to one, if the VLD has detected that not enough decoded macroblock,
		//And that the the frame_num did not change, and if we are still in the same layer
		*end_of_slice = PrevEndOfSlice && Slice -> frame_num == LastFrameNum && FirstMbInSlice != Slice -> first_mb_in_slice && Nal -> DqId == Nal -> LastDqId? 1: 0;



		Mmo -> LayerId = Nal -> LayerId;
		
		//Read Frame parameter
		FrameHeaderData(data, position, Nal -> NalRefIdc, Nal -> IdrFlag, Nal -> LayerId, *end_of_slice, 
			Current_pic, pt_sps_id, pt_pps_id,Slice);

		dep_mmo->frame_num = Slice->frame_num;
		//Compute POC and get memory addresses
		if ( !*end_of_slice){
			LayerMmo->slice = Nal->pSlice;
			if(!dep_mmo->poc_cal_done)
			{
				pic_order_process(pt_sps_id, Nal -> IdrFlag * 5, Nal -> NalRefIdc, Mmo, 
					LayerMmo, Current_pic, RefPicListL0, RefPicListL1,Slice,dq_id_display);
				dep_mmo->poc_cal_done = 1;
				dep_mmo->poc = Current_pic->poc;
			}
			else
			{
				Current_pic->poc = dep_mmo->poc;
			}
			Current_pic->memory_size = (Mmo->MaxWidth+32)*(Mmo->MaxHeight+32);
			Current_pic->pic_width = pt_sps_id->pic_width_in_mbs*16;
			Current_pic->pic_height = pt_sps_id->pic_height_in_map_units*16;
			allocate_picture_memory(Current_pic);
			Current_pic -> displayed = 0;
		}

		

		//Save the memory 
		Nal->current_picture = Current_pic;

		//Get the reference listes
		SvcReadReferenceListProcessing(data, position, Nal, Slice, pt_sps_id, pt_pps_id, LayerMmo,
			Current_pic, Mmo, RefPicListL0, RefPicListL1);
		
		
		if (!*end_of_slice){
			direct_ref_list_init(Slice, RefPicListL0, RefPicListL1, Current_pic, LayerMmo);
		}

		//Parse Qp, deblocking information and inter-layer prediction.
		QpAndDeblockProcessing(data, position, Slice, pt_pps_id, pt_sps_id);
		SvcReadLayerPrediction(data, position, Nal, pt_sps_id, pt_pps_id, Slice, Current_pic, Mmo, *end_of_slice);
		
		
		//Initialize up-sampling parameters
		NalParamInit(Nal, pps, sps, pt_sps_id);
		if(Nal -> LayerId && !*end_of_slice && Nal -> UsedSpatial [Nal -> BaseLayerId][0] ==  -1){
			SetSpatialUsed(Nal);
		}

		//Initialize the Nal decoding process parameters
		SetPicToDisplay(Nal);
		IsPicToDecode(Nal);


		//Read default parameters for VLD.
		SvcReadDefaultParameters(data, position, Nal, Slice, pt_sps_id);


		Nal -> SpatialScalabilityType = SpatialScalabilityType(pt_sps_id, Nal);
		init_quantization (quantif, pt_sps_id -> scaling_matrix_present, pt_pps_id -> scaling_matrix4, pt_pps_id -> scaling_matrix8 );


		//Ghost Picture Detected
		if( Nal -> NalToSkip){
			Current_pic -> displayed = 255;
		}

		Nal -> LastLayerId = Nal -> LayerId;
		Nal -> LastDqId = Nal -> DqId;
		Nal -> LastTemporalId = Nal -> TemporalId;
		Mmo -> nb_img_display = sps[SeqParameterId] . nb_img_disp;
		Slice -> slice_num++ ;
		*pps_id = pps[PicParameterId];
		*sps_id = sps[SeqParameterId];
		LayerMmo->pcurr_sps = &(sps[SeqParameterId]);
		LayerMmo->pcurr_pps = &(pps[PicParameterId]);
		LayerMmo->slice = Nal->pSlice;
		LayerMmo->nal = Nal;

	}
	else {
		Nal -> NalToSkip = 1;
		Nal -> PicToDecode = Nal -> PicToDisplay = 0;
	}

	return 0;
}

//for init access unit
int slice_header_svc_cut(unsigned char *data, SPS *sps, PPS *pps, int *position, 
						 SLICE *Slice, NAL *Nal)
{
		PPS *pt_pps_id ;
		SPS *pt_sps_id ;	
		int PicParameterId;
		int SeqParameterId;

		//Read header data
		Slice -> first_mb_in_slice = read_ue(data, position);

		Slice -> slice_type = read_ue(data, position);
		if ( Slice -> slice_type > 4 ) {
			Slice -> slice_type -= 5 ;
		}

		PicParameterId = read_ue(data, position);
		Nal -> pic_parameter_id[Nal -> LayerId] = PicParameterId = CLIP3(0, PPS_BUF_SIZE - 1, PicParameterId);
		pt_pps_id = &pps[PicParameterId];
		//Should be in slice header
		//Sometimes Subset SPS are after PPS (rtsp)
		if(pt_pps_id -> seq_parameter_set_id > sps[0] . MaxSpsId){
			pt_pps_id -> seq_parameter_set_id = sps[0] . MaxSpsId;
		}

		



		SeqParameterId = pt_pps_id -> seq_parameter_set_id + (Nal -> LayerId ? 16: 0);
		pt_sps_id = &sps[SeqParameterId];


		Slice -> frame_num = getNbits(data, position, pt_sps_id -> log2_max_frame_num );
		//Read Frame parameter
		if ( !pt_sps_id -> frame_mbs_only_flag ) {
			Slice -> field_pic_flag = getNbits(data, position, 1);
			if ( Slice -> field_pic_flag ) {
				Slice->bottom_field_flag = getNbits(data, position, 1);
			}
		}
		else
		{
			Slice->field_pic_flag = 0;
		}
		if ( 1 == Nal -> IdrFlag) {
			Slice->idr_pic_id = read_ue(data, position);//idr_pic_id =	
		}
		if ( pt_sps_id -> pic_order_cnt_type == 0 ) {
			Slice -> pic_order_cnt_lsb = getNbits(data, position, pt_sps_id -> log2_max_pic_order_cnt_lsb ); 
			if ( pt_pps_id -> pic_order_present_flag && !Slice -> field_pic_flag ) 
				Slice -> delta_pic_order_cnt_bottom = read_se(data, position);
		}

		if ( pt_sps_id -> pic_order_cnt_type == 1 && !pt_sps_id -> delta_pic_order_always_zero_flag ) {
			Slice -> delta_pic_order_cnt [0] = read_se(data, position); 
			if ( pt_pps_id -> pic_order_present_flag && !Slice -> field_pic_flag ) 
				Slice -> delta_pic_order_cnt [1] = read_se(data, position);
		}

		if ( pt_pps_id -> redundant_pic_cnt_present_flag ) {
			read_ue(data, position);//redundant_pic_cnt =
		}


	return 0;
}



