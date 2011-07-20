
//AVC Files
#include "type.h"
#include "data.h"
#include "bitstream.h"

#include "clip.h"



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



