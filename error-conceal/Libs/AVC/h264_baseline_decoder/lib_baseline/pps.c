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
#include "math.h"


#include "type.h"
#include "clip.h"
#include "bitstream.h"
#include "slice_data_vlc.h"
#include "scaling_matrix.h"
#include "stdlib.h"








/**
This function permits to decode from the bitstream the Pictures Parameter Set NAL unit. 
All the parameters decoded will be stored in the pps structure.

@param data The NAL unit.
@param Position The current Position in the NAL.
@param ao_aio_pstSlice_group_id Tab which identifies the i-th aio_pstSlice group.
@param pps The pps structure which contains all parameters decoded in this NAL.
@param NalBytesInNalunit Size of the current Nal.
*/


void pic_parameter_set  (NAL* nal, PPS *pps, SPS *sps)
{
	int	i;
	int	iGroup;
	int	Position = 8;
	unsigned char* data = nal->prbsp_memory;
	unsigned char Pps_id = read_ue(data, &Position);
	unsigned char Sps_id = read_ue(data, &Position);
	unsigned char pic_parameter_set_id = CLIP3(0, PPS_BUF_SIZE - 1, Pps_id);
	unsigned char seq_parameter_set_id = CLIP3(0, SPS_BUF_SIZE - 1, Sps_id);
	PPS *pt_pps = &pps[pic_parameter_set_id];
	SPS *pt_sps = &sps[seq_parameter_set_id];

	pt_pps -> entropy_coding_mode_flag = getNbits(data, &Position, 1);
	pt_pps -> seq_parameter_set_id = seq_parameter_set_id;
    pt_pps -> pic_order_present_flag = getNbits(data, &Position, 1);
    pt_pps -> num_slice_groups_minus1 = read_ue(data, &Position);

	//Initialization of the maximun number of pps
    if(pic_parameter_set_id > pps[0] . MaxPpsId){
		pps[0] . MaxPpsId = pic_parameter_set_id;
	}

    if ( pt_pps -> num_slice_groups_minus1 > 0 ) {
        pt_pps -> slice_group_map_type = read_ue(data, &Position);
        if ( pt_pps -> slice_group_map_type == 0 ) 
            for ( iGroup = 0 ; iGroup <= pt_pps -> num_slice_groups_minus1 ; iGroup++ ) 
                pt_pps -> run_length_minus1 [iGroup] = read_ue(data, &Position);
        else if ( pt_pps -> slice_group_map_type == 2 ) 
            for ( iGroup = 0 ; iGroup < pt_pps -> num_slice_groups_minus1 ; iGroup++ ) {
                 pt_pps -> top_left_tab [iGroup] = read_ue(data, &Position);//
                 pt_pps -> bottom_right_tab [iGroup] = read_ue(data, &Position);//
            }
        else if ( pt_pps -> slice_group_map_type == 3 || pt_pps -> slice_group_map_type == 4
                    || pt_pps -> slice_group_map_type == 5 ) {
            pt_pps -> slice_group_change_direction_flag = getNbits(data, &Position, 1);
            pt_pps -> slice_group_change_rate_minus1 = read_ue(data, &Position) + 1;
        } else if ( pt_pps -> slice_group_map_type == 6 ) {
            short log2_num_slice_groups_minus1 = log_base2(pt_pps -> num_slice_groups_minus1 + 1);
            short pic_size_in_map_units_minus1 = read_ue(data, &Position);
			for ( i = 0 ; i <= pic_size_in_map_units_minus1 ; i++ ) {
				if (pt_pps->PicSizeInMapUnits<pic_size_in_map_units_minus1+1)
				{
					pt_pps->PicSizeInMapUnits = pic_size_in_map_units_minus1+1;
					free(pt_pps->slice_group_id);
					pt_pps->slice_group_id = (unsigned char*) malloc(pt_pps->PicSizeInMapUnits);
				}
                pt_pps->slice_group_id [i] = getNbits(data, &Position, log2_num_slice_groups_minus1);
			}
        }
    }

    pt_pps -> num_RefIdxL0_active_minus1 = read_ue(data, &Position);
    pt_pps -> num_RefIdxL1_active_minus1 = read_ue(data, &Position);
    pt_pps -> WeightedPredFlag = getNbits(data, &Position, 1);
    pt_pps -> WeightedBipredIdc = getNbits(data, &Position, 2); //pt_pps -> weighted_bipred_idc = 
    pt_pps -> pic_init_qp = read_se(data, &Position) + 26;
    read_se(data, &Position); //pic_init_qs_minus26 =
    pt_pps -> second_chroma_qp_index_offset = pt_pps -> chroma_qp_index_offset = read_se(data, &Position);
    pt_pps -> deblocking_filter_control_present_flag = getNbits(data, &Position, 1);
    pt_pps -> constrained_intra_pred_flag = getNbits(data, &Position, 1);
    pt_pps -> redundant_pic_cnt_present_flag = getNbits(data,  &Position, 1);

	if( more_rbsp_data(data, &Position, nal->nal_byte_size) ) { // Position  < NalBytesInNalunit << 3
		
		pt_pps -> transform_8x8_mode_flag = getNbits(data, &Position, 1); 
		decode_scaling_matrices(data, &Position, pt_sps, 0, pt_pps -> scaling_matrix4, pt_pps -> scaling_matrix8);
		pt_pps -> second_chroma_qp_index_offset = read_se(data, &Position);
		if ( abs(pt_pps -> second_chroma_qp_index_offset) > 12){
			pt_pps -> second_chroma_qp_index_offset = pt_pps -> chroma_qp_index_offset;
		}
	}
    rbsp_trailing_bits(&Position);
}





/**
This functio nallows to initialize the PPS structure.

@param pps PPS structure of the current video.
*/
void init_pps ( PPS *pps )
{
    int j ;
    
	for(j = 0; j < 255; j++){
		memset(&pps[j],'\0', sizeof(PPS));
	}
}

void free_pps_memory(PPS *pps)
{
	int j;
	for (j = 0;j<255;j++)
	{
		if (pps[j].slice_group_id)
		{
			free(pps[j].slice_group_id);
		}
	}
}

