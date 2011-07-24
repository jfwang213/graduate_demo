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

#include "type.h"
#include "clip.h"
#include "sps.h"
#include "bitstream.h"
#include "scaling_matrix.h"






#define Extended_SAR 255

static int MaxDPB [5] [5] = {{396,900,2376,2376},{2376,4752,8100,0},{8100,18000,20480,0},{32768,32768,34816,0},{110400,184320,0}};




void get_max_dpb(SPS * sps_id){

	int dizaine = sps_id -> level_idc / 10;
	int maxDpb = MaxDPB[dizaine - 1][(sps_id -> level_idc - dizaine * 10) & 0x07];
	int nb_ref_frames = maxDpb / sps_id -> PicSizeInMbs;
	sps_id -> nb_img_disp = MIN(nb_ref_frames, 16); 
}





/**
   This function permits to get from the NAL unit additional parameters. 
   
   @param data The NAL unit.
   @param position The current position in the NAL.
   */
void hrd_parameters ( unsigned char *data, int *position, SPS *sps )
{

	int             SchedSelIdx ;

    
    sps -> cpb_cnt = read_ue(data, position) + 1;
    getNbits(data, position, 4);//bit_rate_scale =
    getNbits(data, position, 4);// cpb_size_scale =
    for ( SchedSelIdx = 0 ; SchedSelIdx <= sps -> cpb_cnt -1 ; SchedSelIdx++ ) {
      read_ue(data, position);// /*  bit_rate_value_minus1 [SchedSelIdx] = */
      read_ue(data, position);///*  cpb_size_value_minus1 [SchedSelIdx] = */
      getNbits(data, position, 1);///*   cbr_flag [SchedSelIdx] =  */
    }

    sps -> initial_cpb_removal_delay_length = getNbits(data, position, 5) + 1;
    sps -> cpb_removal_delay_length = getNbits(data, position, 5) + 1;// = 
    getNbits(data, position, 5);// dpb_output_delay_length_minus1 =
    sps -> time_offset_length = getNbits(data, position, 5);//time_offset_length =
}





/**
   This function permits to get from the NAL unit additional parameters. 
   VUI parameters are not required for constructing the luma and chroma samples by teh decoding process.
   
   @param data The NAL unit.
   @param position The current position in the NAL.
   */
void vui_parameters ( unsigned char *data, int *position, SPS *sps )
{

    int    overscan_info_present_flag ;
    int    video_signal_type_present_flag ;
    int    bitstream_restriction_flag ;


    int aspect_ratio_info_present_flag = getNbits(data, position, 1);
    if ( aspect_ratio_info_present_flag ) {
       int  aspect_ratio_idc = getNbits(data, position, 8);
        if ( aspect_ratio_idc == Extended_SAR ) {
            getNbits(data, position, 16);//sar_width = 
            getNbits(data, position, 16);// sar_height = 
        }
    }
    //scanner dans ffmpeg mais pas pris en compte
    overscan_info_present_flag = getNbits(data, position, 1);
    if ( overscan_info_present_flag ) 
        getNbits(data, position, 1);// overscan_appropriate_flag =

    video_signal_type_present_flag = getNbits(data, position, 1);
    if ( video_signal_type_present_flag ) {
        getNbits(data, position, 3);//video_format =
        getNbits(data, position, 1);// video_full_range_flag =
        if (getNbits(data, position, 1)) {
           getNbits(data, position, 8);// colour_primaries = 
           getNbits(data, position, 8);// transfer_characteristics =
           getNbits(data, position, 8);//  matrix_coefficients = 
        }
    }

    if (getNbits(data, position, 1)) {
        read_ue(data, position);//chroma_sample_loc_type_top_field =
        read_ue(data, position);// chroma_sample_loc_type_bottom_field =
    }


    //******************************************
    if (getNbits(data, position, 1)) {
       get32bits(data, position);// num_units_in_tick = 
       get32bits(data, position);//time_scale = 
       getNbits(data, position, 1);// fixed_frame_rate_flag =
    }


    sps -> nal_hrd_parameters_present_flag = getNbits(data, position, 1);
    if ( sps -> nal_hrd_parameters_present_flag ) 
        hrd_parameters(data, position,sps);

    sps -> vlc_hrd_parameters_present_flag = getNbits(data, position, 1);
    if ( sps -> vlc_hrd_parameters_present_flag ) 
        hrd_parameters(data, position,sps);

    //scanner dans ffmpeg mais pas pris en compte
    if ( sps -> nal_hrd_parameters_present_flag ||  sps -> vlc_hrd_parameters_present_flag ) 
        getNbits(data, position, 1);//low_delay_hrd_flag = 

    //********************************************
    sps -> pic_present_flag = getNbits(data, position, 1);//  =
    bitstream_restriction_flag = getNbits(data, position, 1);
    if ( bitstream_restriction_flag ) {
        //scanner dans ffmpeg mais pas pris en compte
        getNbits(data, position, 1);// motion_vectors_over_pic_boundaries_flag =
        read_ue(data, position);//max_bytes_per_pic_denom =
        read_ue(data, position);//max_bits_per_mb_denom = 
        read_ue(data, position);//log2_max_mv_length_horizontal = 
        read_ue(data, position);//log2_max_mv_length_vertical =
        //******************************************
        read_ue(data, position);//num_reorder_frames = 
        read_ue(data, position);//max_dec_frame_buffering =
    }
}








/**
   This function permits to decode from the bitstream the Sequence Parameter Set NAL unit. 
   All the parameters decoded will be stored in the sps structure.
   
   @param data The NAL unit.
   @param position The current position in the NAL.
   @param NalBytesInNalunit The sizes of the NAL unit in bytes.
   @param sps The sps structure which contains all parameters decoded in this NAL.
   */
void seq_parameter_set_data ( unsigned char *data, int *position,  SPS *pt_sps){
    
	
	
	int     i = 0 ;
	
	if( pt_sps -> profile_idc  ==  100  || pt_sps ->  profile_idc  ==  110  ||	
		pt_sps -> profile_idc  ==  122  || pt_sps ->  profile_idc  ==  244  ||
		pt_sps -> profile_idc  ==  44 ||  pt_sps ->  profile_idc  ==  83 ||	pt_sps -> profile_idc  ==  86 ) {
		pt_sps -> chroma_format_idc = read_ue(data, position);
		if( pt_sps -> chroma_format_idc  ==  3 ){
			getNbits(data, position, 1);//separate_colour_plane_flag
		}
		pt_sps -> bit_depth_luma = read_ue(data, position) + 8; 
		pt_sps -> bit_depth_chroma = read_ue(data, position) + 8;
		getNbits(data, position, 1);//qpprime_y_zero_transform_bypass_flag
		decode_scaling_matrices(data, position, pt_sps,1, pt_sps -> scaling_matrix4	, pt_sps -> scaling_matrix8);
	}



    pt_sps -> log2_max_frame_num = read_ue(data, position) + 4;
    pt_sps -> pic_order_cnt_type = read_ue(data, position);

    if ( pt_sps -> pic_order_cnt_type == 0 ) {
        pt_sps -> log2_max_pic_order_cnt_lsb = read_ue(data, position) + 4;
    } else if ( pt_sps -> pic_order_cnt_type == 1 ) {
        pt_sps -> delta_pic_order_always_zero_flag = getNbits(data, position, 1);
        pt_sps -> offset_for_non_ref_pic = read_se(data, position); 
        pt_sps -> offset_for_top_to_bottom_field = read_se(data, position);
        pt_sps -> num_ref_frames_in_pic_order_cnt_cycle =  read_ue(data, position);

        for (; i < pt_sps -> num_ref_frames_in_pic_order_cnt_cycle ; i++ ) {
            pt_sps -> offset_for_ref_frame [i] = read_se(data, position);
        }
    }

    pt_sps -> num_ref_frames = read_ue(data, position);// 
    getNbits(data, position, 1);//gaps_in_frame_num_value_allowed_flag = 
    pt_sps -> pic_width_in_mbs = read_ue(data, position) + 1;
    pt_sps -> pic_height_in_map_units = read_ue(data, position) + 1;
    pt_sps -> frame_mbs_only_flag = getNbits(data, position, 1);
	pt_sps -> PicSizeInMbs =  pt_sps -> pic_width_in_mbs * pt_sps -> pic_height_in_map_units;

    if ( !pt_sps -> frame_mbs_only_flag ) {
        pt_sps -> MbAdaptiveFrameFieldFlag = getNbits(data, position, 1);
    }
	
	//To initialize the number of decoded frame before displaying.
	get_max_dpb(pt_sps);

    pt_sps -> direct_8x8_inference_flag = getNbits(data, position, 1);

    if ( getNbits(data, position, 1)) {//frame_cropping_flag 
        pt_sps -> CropLeft = (read_ue(data, position)) << 1;//frame_crop_left_offset = 
        pt_sps -> CropRight = (read_ue(data, position)) << 1;//frame_crop_right_offset = 
        pt_sps -> CropTop = (read_ue(data, position)) << 1;//frame_crop_top_offset = 
        pt_sps -> CropBottom = (read_ue(data, position)) << 1;//frame_crop_bottom_offset = 
    }
  
	if ( getNbits(data, position, 1) ) {
        vui_parameters(data, position, pt_sps);
	}
}



void seq_parameter_set( unsigned char *data, SPS *sps){


	int	position = 8;
	int profile_idc = getNbits(data, &position, 8);
    int constraint = getNbits(data, &position, 8);
    int level_idc = getNbits(data, &position, 8); //level_idc =

    if (profile_idc && !(constraint & 0xf)){
		char Sps_id = read_ue(data, &position);
		char seq_parameter_set_id = CLIP3(0, SPS_BUF_SIZE, Sps_id);
		SPS *pt_sps = &sps [seq_parameter_set_id];
		
		//To save parameter before resetting
		short b_stride = pt_sps -> b_stride; 
		short b8_stride = pt_sps -> b8_stride; 
		
		//Initialization of the maximun number of sps
		if(seq_parameter_set_id > sps[0] . MaxSpsId){
			sps[0] . MaxSpsId = seq_parameter_set_id;
		}

		//Intilaize the SPS to zero
		memset(pt_sps, 0, sizeof(SPS));
		pt_sps -> b_stride = b_stride; 
		pt_sps -> b8_stride = b8_stride; 
		pt_sps -> profile_idc = profile_idc;
		pt_sps -> level_idc = level_idc;


		seq_parameter_set_data(data, &position, pt_sps);
		pt_sps -> slice_header_restriction_flag = -1;
		rbsp_trailing_bits(&position);
	}
}



void init_sps ( SPS *sps ){
    
	int i;
	for( i = 0; i < 32; i++){
		memset (&sps[i],'\0',sizeof(SPS));
	}
}
