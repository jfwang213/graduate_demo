//To activate error detection
//#define ERROR_DETECTION
//#define ERROR_CONCEALMENT
//#define ENABLE_PACKET_LOSS_DETECTION_

#include "list_head.h"
#include "list_wrap.h"
#ifndef NULL

#define NULL 0

#endif
#ifndef TYPE_H
#define TYPE_H


#ifndef WIN32
#define __declspec(X) 
#endif


#define RESTRICT

#ifdef TI_OPTIM
#define PIC_WIDTH  720
#define PIC_HEIGHT 576
#undef RESTRICT
#define RESTRICT restrict
#define UINT64 unsigned long long 
#elif POCKET_PC
#define PIC_WIDTH  640
#define PIC_HEIGHT 480
#define UINT64 unsigned __int64
#else
#pragma warning (disable : 177)
//#define UINT64 unsigned __int64
#endif


#ifdef MMXi
#ifdef POCKET_PC
#include <mmintrin.h>
#include <cmnintrin.h> 
#define empty() 
#else
#include <emmintrin.h>
#include <mmintrin.h>
#define empty _mm_empty
#endif
#endif


#ifdef SSE2
#ifdef POCKET_PC
#include <mmintrin.h>
#include <cmnintrin.h> 
#else
#include <emmintrin.h>
#include <mmintrin.h>
#endif
#endif


#define MAX_SIZE_DBP (PIC_WIDTH + 32)*(PIC_HEIGHT + 32)*DBP_DEPTH
#define PIC_WIDTH_MB_MAX (PIC_WIDTH >> 4)
#define PIC_HEIGHT_MB_MAX (PIC_HEIGHT >> 4)
#define NUM_SLICE_GROUP_MINUS_MAX 8
#define MAX_MMCO_COUNT 66
#define SPS_BUF_SIZE 32
#define PPS_BUF_SIZE 255
#define LIST_MMO_REFPIC_SIZE 16
#define MAX_GOP				16

//SVC
#define MAX_SPATIAL_LEVELS 3
#define MAX_TEMP_LEVELS 8
#define MAX_QUALITY_LEVELS 3
#define MAX_SCALABLE_LAYERS MAX_SPATIAL_LEVELS * MAX_TEMP_LEVELS * MAX_QUALITY_LEVELS

#define REF_LUMA_BUF_SIZE 52469760		//arbitary value
#define REF_CHROMA_BUF_SIZE 13117440	//arbitary value

#define MAX_SLICES_PER_FRAME 32 // maximum number of slices per frame with packet loss detection

//Memory Management

#define NUM_POC_PARAMS 24
#define NUM_LAYERS 8
#define NUM_DEP_LAYERS 4
#define MAX_REF 16
#define NUM_FREE_MEM 32
#define NUM_MV 32

#define MAX_WIDTH (704)
#define MAX_HEIGHT (576)
#define PIC_EDGE   (16)
#define nal_bytes (1013760)
#define nal_rbsp_bytes (1013760)
#define max_layer	(6)

#define all_mv_memory (MAX_WIDTH*MAX_HEIGHT/8*max_layer*NUM_MV)
#define all_ref_memory (MAX_WIDTH*MAX_HEIGHT/8*max_layer*NUM_MV/2)
#define one_pic_memory (8+(MAX_WIDTH+PIC_EDGE*2)*(MAX_HEIGHT+PIC_EDGE*2)/4*6)
#define all_pic_memory ((MAX_WIDTH+32)*(MAX_HEIGHT+32)*(NUM_FREE_MEM+max_layer))
#define all_resi_memory ((MAX_WIDTH+32)*(MAX_HEIGHT+32)*NUM_LAYERS)
#define block_num	(MAX_WIDTH*MAX_HEIGHT>>6)
#define MB_NUM		(MAX_WIDTH*MAX_HEIGHT>>8)
#define one_pic_sample (MAX_WIDTH*MAX_HEIGHT)






//Define structure
typedef struct 
{

	int offset_for_non_ref_pic;
	int offset_for_top_to_bottom_field;
	int offset_for_ref_frame[255];

	
	short max_pic_num;

	//Pic size
	short pic_width_in_mbs ;
	short pic_height_in_map_units ;
	short PicSizeInMbs;
	short b_stride ;
	short b8_stride;
	short scaled_base_left_offset;
	short scaled_base_top_offset;
	short scaled_base_right_offset;
	short scaled_base_bottom_offset;

	
	//Crop
	short CropLeft; 
    short CropRight;
    short CropTop;
    short CropBottom; 

	//Scaling matrix
	short scaling_matrix_present;
	short  scaling_matrix4[6][16];
	short  scaling_matrix8[2][64];

	unsigned char profile_idc;
	unsigned char level_idc;
	unsigned char pic_order_cnt_type ;
	unsigned char frame_mbs_only_flag ;
	unsigned char delta_pic_order_always_zero_flag ;
	unsigned char log2_max_frame_num ;
	unsigned char log2_max_pic_order_cnt_lsb;
	unsigned char MbAdaptiveFrameFieldFlag ;
	unsigned char direct_8x8_inference_flag;	
	unsigned char num_ref_frames_in_pic_order_cnt_cycle;
	unsigned char num_ref_frames;
	unsigned char nb_img_disp;
	unsigned int max_frame_num;


	//SEI
	unsigned char nal_hrd_parameters_present_flag;
	unsigned char vlc_hrd_parameters_present_flag;
	unsigned char cpb_cnt;
	unsigned char cpb_removal_delay_length;
	unsigned char pic_present_flag;
	unsigned char time_offset_length;
	unsigned char initial_cpb_removal_delay_length;

	


	//HP
	unsigned char bit_depth_luma;
	unsigned char bit_depth_chroma;
	

	//SVC
	unsigned char extended_spatial_scalability;
	char slice_header_restriction_flag; 
	unsigned char InterLayerDeblockingFilterFlag;
	unsigned char adaptive_tcoeff_level_prediction_flag;
	unsigned char tcoeff_level_prediction_flag;
	char chroma_phase_x;
	char chroma_phase_y;
	unsigned char chroma_format_idc;

	//Error detection
	unsigned char MaxSpsId;

}   SPS ;








typedef struct {

	//FMO tab
	short run_length_minus1 [NUM_SLICE_GROUP_MINUS_MAX];
	short bottom_right_tab[NUM_SLICE_GROUP_MINUS_MAX];
	short top_left_tab[NUM_SLICE_GROUP_MINUS_MAX];

	//SCALING MATRIX
	short scaling_matrix8[2][64];
	short scaling_matrix4[6][16];
	short slice_group_change_rate_minus1 ;


	unsigned char seq_parameter_set_id ;
	unsigned char pic_init_qp;
	char chroma_qp_index_offset ;
	unsigned char num_slice_groups_minus1 ;
	unsigned char pic_order_present_flag ;
	unsigned char redundant_pic_cnt_present_flag ;
	unsigned char WeightedPredFlag ;
	unsigned char WeightedBipredIdc ;
	unsigned char deblocking_filter_control_present_flag ;
	unsigned char slice_group_map_type ;
	unsigned char entropy_coding_mode_flag ;
	unsigned char num_RefIdxL0_active_minus1 ;
	unsigned char num_RefIdxL1_active_minus1 ;

	unsigned char slice_group_change_direction_flag ;
	unsigned char constrained_intra_pred_flag ;
	

	//HP 
	unsigned char transform_8x8_mode_flag;
	char second_chroma_qp_index_offset;
	char alpha_offset;
	char beta_offset;

	//SVC
	unsigned char constrained_intra_upsampling_flag;
	char alpha_offset_upsapmpling;
	char beta_offset_upsapmpling;
	
	//Error detection
	unsigned char MaxPpsId;

	//slice_group_map_type = 6
	unsigned char *slice_group_id;
	short PicSizeInMapUnits;
}   PPS ;





typedef struct {
	short dist_scale_factor[16];
	short MapColList[32];

	//slice info
	unsigned char field_pic_flag ;
	unsigned char bottom_field_flag;

	//MB size and managment
	short b_stride ;
	short b8_stride ;
	short mb_xy;
	short mb_stride ;
	short first_mb_in_slice ;
	short slice_group_change_cycle ;


	//poc
	//type 0
	short pic_order_cnt_lsb;
	short delta_pic_order_cnt_bottom;
	//type 1
	short delta_pic_order_cnt[2];

	unsigned char slice_num ;
	char slice_qp_delta ;
	unsigned char slice_type ;
	unsigned char cabac_init_idc;
	int idr_pic_id;

	//frame num
	unsigned int frame_num;
	unsigned int frame_num_offset;
	
	unsigned char num_RefIdxL0_active_minus1;
	unsigned char num_RefIdxL1_active_minus1;
	unsigned char disable_deblocking_filter_idc;
	unsigned char direct_spatial_mv_pred_flag;

	// SVC
	unsigned char AdaptiveBaseModeFlag;
	unsigned char DefaultBaseModeFlag;
	unsigned char AdaptivePredictionFlag;
	unsigned char AdaptiveResidualPredictionFlag;
	unsigned char DefaultResidualPredictionFlag;
	unsigned char AdaptiveMotionPredictionFlag;
	unsigned char DefaultMotionPredictionFlag;
	unsigned char notDefaultScanIdx;
	unsigned char InterLayerDeblockingFilterIdc;

#ifdef ENABLE_PACKET_LOSS_DETECTION
	// variables for packet loss detection
	short first_mb[MAX_SLICES_PER_FRAME];
	short first_mb_index;
	short first_mb_in_slice_expected;
	short num_of_slices_per_frame;
	short slice_cycle_ready;
#endif

	//slice group map
	unsigned char *mb_slice_group_map;
	unsigned char *mb_slice_table;

}   SLICE;


typedef struct 
{
	//Memory addresses
	unsigned char* picture_memory_y;
	unsigned char* picture_memory_u;
	unsigned char* picture_memory_v;
	short* residual_memory_y;
	short* residual_memory_u;
	short* residual_memory_v;
	short* mv_memory_l0;
	short* ref_memory_l0;
	short* mv_memory_l1;
	short* ref_memory_l1;
	int memory_size;
	int pic_width;
	int pic_height;
	//size of picture_memory_y = memory_size;
	//size of mv_memory_lx = memory_size/16*2
	//size of ref_memory_lx = memory_size/16
	//size of residual_memory_y = memory_size, as a result of short

	//frame parameters
	short poc; // picture order count
	short pic_num; 
	short pic_id;
	
	short ref_poc_l0[16];//poc of reference frame
	short ref_poc_l1[16];

	

	unsigned char long_term;
	unsigned char displayed;
	char reference;
	char ref_count_l0;
	char ref_count_l1;
	char bBasePic;
	//release memory
	unsigned char ucLayerId;


	//Player
	unsigned char KeyFrame;
	unsigned char PicToDisplay;

	//for interlayer prediction duplication variable
	unsigned char slice_type;

} LIST_MMO;


typedef struct
{
	unsigned char* display_y;
	unsigned char* display_u;
	unsigned char* display_v;
	char	to_release;
	int memory_size;
} DISPLAY_M;


typedef struct
{
	int Crop;

	short x_size;
	short y_size;
	
	unsigned char is_ghost;
	unsigned char LayerId;

	LIST_MMO Picture [1];
} POC_PARAMS;


typedef struct {
	//access unit
	list_item nal_list[1];

	//Layer Managment
	int TemporalId;
	int LastTemporalId;  //Temporal_id of the last decoded nal
	int LastLayerId;	 //number of layer of the last decoded nal
	int BaseLayerId;	 //number of layer of the base nal
	int LayerId;		 //number of layer of the current nal
	int DependencyId;
	int QualityId;
	int DqId;			 //DqId of the current nal
	int LastDqId;		 //DqId of the last decoded nal
	int NextDqId;		 //DqId of the next nal (only in the decoder, not players)
	int DqIdToDisplay;   //DqId of the nal to display
	int DqIdWanted;	     //DqId requested by the user
	int PicToDecode;	 //Frame used for prediction. Several algorithms have to be done for upper layer.
	int PicToDisplay;	 //Frame have to be totally decoded (Motion ompensation, ...)
	int NalToSkip;	 //Nal have to be decoded. Used for temporal scalability 

	//Stream configuration
	int* pic_parameter_id;
	char* layer_id_table;
	char* UsedSpatial[8];
	//char MaxNumLayer;	 //Number of different layer allowed by the decoder
	//int DependencyIdMax; //Maximum dependency_Id found in the stream
	//int MaxTemporalId;   //Maximum temporal_id found in the stream
	//int DqIdMax;		 //Maximum DqId found in the stream



	//Nal type
	int NalRefIdc;
	int NalUnitType;
	int IdrFlag;


	//SVC Nal configuration
	int layer_base_flag;
	int UseRefBasePicFlag;
	int no_inter_layer_pred_flag;
	int StoreRefBasePicFlag;
	int DiscardableFlag;
	int SpatialScalabilityType;
	int non_diadic;
	short SCoeffPrediction;
	short TCoeffPrediction;
	short SliceSkipFlag;


	//Frame memory addresses
	//int LastMemoryAddress;			//Memory address of the frame decoded in the last nal
	//int BaseMemoryAddress;			//Memory address of the base layer
	//int BaseMvMemoryAddress;		//MV memory address of the base layer
	//int MemoryAddress;				//Current frame memory address
	//int MvMemoryAddress;			//Current frame Mv memory address
	LIST_MMO* base_picture;
	LIST_MMO* current_picture;
	//Frame crop
	int b_stride ;
	int b8_stride ;
	int is_cropping;
	int left_offset ;
	int top_offset;

	SLICE* pSlice;


	//Frame size
	short curr_Width;
	short curr_Height;
	short up_Width;
	short up_Height;
	short base_Width;
	short base_Height;
	short down_Width;
	short down_Height;


	//Loop Filter
	short SpatialScalability; // difference of macroblock
	short LoopFilterSpatialScalability; //the same size for the common area (crop)
	short LoopFilterForcedIdc;

	//Error Detection
	char ErrorDetected;
	char ErrorDetection;

	//slice management
	char bFinishLayerRepresent;
	char bFinishDependenceRepresent;
	char bFinishAccessUnit;

	//rbsp
	unsigned char* prbsp_memory;
	int		nrbsp_memory_size;
	int		nal_byte_size;
	int		current_position[1];

}   NAL ;


typedef struct MMCO{

	short short_frame_num;
	short long_term_frame_idx;
	unsigned char opcode;
} MMCO;

typedef struct {

	int AvailMask;
	int blk4x4;
	int blk4x4_Res;
	int Cbp;

	//Residu stuff
	short Intra16x16DCLevel [16]; //Used also for I prediction mode in case of 4x4 and 8x8 blocks
	short LumaLevel [16 * 16];
	short ChromaDCLevel_Cb [4];
	short ChromaDCLevel_Cr [4];
	short ChromaACLevel_Cb [4 * 16];
	short ChromaACLevel_Cr [4 * 16];


	unsigned char MbType ;
	unsigned char Mode;
	unsigned char Intra16x16PredMode ;
	unsigned char IntraChromaPredMode ;
	unsigned char SubMode;
	unsigned char SubMbType [4];
	unsigned char SliceNum;

	unsigned char Qp;
	unsigned char QpLf;
	unsigned char Transform8x8;
	unsigned char ResidualPredictionFlag;
	unsigned char BaseModeFlag;
	unsigned char Is_Intra4x4;
	unsigned char InCropWindow;
}   RESIDU ;


typedef struct  
{
	LIST_MMO LongRef[MAX_REF]; // 16
	LIST_MMO ShortRef[MAX_REF]; // 16
	LIST_MMO LongRefBase[MAX_REF]; // 16
	unsigned char index;
	unsigned char baseLayerIndex;
	MMCO Mmco[MAX_MMCO_COUNT];
	MMCO BaseLayerMmco[MAX_MMCO_COUNT];
	unsigned char ShortRefCount;
	unsigned char LongRefCount;
	unsigned char LongRefCountBase;
	unsigned char RefCountL0; 
	unsigned char RefCountL1;
	LIST_MMO refl0[16];
	LIST_MMO refl1[16];

	//for dep
	char store_ref_base_pic_flag;
	char idr_flag;
	char do_ref_pic_mark;
	char nal_ref_idc;
	char receive_one_layer;
	char frame_num_gap_check;
	char active;
	char poc_cal_done;
	char last_nal_ref_idc;
	short prevPicOrderCntMsb;
	short prevPicOrderCntLsb;
	short prev_frame_num_offset;
	unsigned int prev_frame_num;
	short frame_num;
	short poc;
}DEP_MMO;

typedef struct 
{

	list_wrap      eclist[1];
	//structure d'image
	DEP_MMO*			dep_mmo;
	unsigned char	prev_pps_id;
	
	int num_decoded_frame;
	
	
	

	LIST_MMO currentPic[1];
	RESIDU residu[MB_NUM];
	SLICE* slice;
	SPS* pcurr_sps;
	PPS* pcurr_pps;
	NAL* nal;

	//layer share data
	unsigned char * mb_slice_group_map;
	unsigned char * mb_slice_table;
	unsigned char  receive_new_nal;
	unsigned char  error_concealment_done;
	unsigned short PicSizeInMapUnits;
	
	
	


} LAYER_MMO;


#define get_nal_from_list(plist_item) contain_of(plist_item,NAL,nal_list)


typedef struct 
{

	POC_PARAMS poc_params[NUM_POC_PARAMS]; //16 + 8 layers
	LAYER_MMO LayerMMO[NUM_LAYERS];
	DEP_MMO	DepMMO[NUM_DEP_LAYERS];
	
	short last_poc;
	// flag that indicates packet loss error
	short LastDecodedPoc;


	//to remove Multithread pb
	short MaxHeight;
	short MaxWidth;

	SPS* pcurr_sps;
	PPS* pcurr_pps;


	char AuMemory;
	char num_free_poc_address;
	char nb_img_display;
	unsigned char LayerId;


	//Player
	POC_PARAMS Prev[1];
	unsigned char low_delay;
	unsigned char has_b_frames;
	unsigned char NumDelayedPics;

	

	//Error detection
	unsigned char MemoryAllocation;
	char* layer_id_table;
} MMO;

typedef struct {
	short MvdL0 [16][2];
	short MvdL1 [16][2];

	//Base Macroblock address
	short xBase;
	short yBase;
	short MbBaseAddr;
	short xBasePart;
	short yBasePart;


	unsigned char NumMbPart ;
	unsigned char MbPartPredMode [2];
	
	//Only for P and B frame
	char RefIdxL0 [4];
	char RefIdxL1 [4];

	unsigned char Transform8x8;

	//SVC
	char MotionPredL0[4];
	char MotionPredL1[4];

	//NonZeroCount	
	unsigned char NonZeroCount[48];

}   DATA ;



typedef struct {
	short   xN ;
	short   yN ;
	short   Avail ;
}   NEIGHBOUR ;










typedef struct {
	short code ;
	short len ;
}   VLC_TYPE ;

typedef struct {
	VLC_TYPE Chroma_dc_coeff_token_vlc [256];
	VLC_TYPE Chroma_dc_total_zeros_vlc [24];
	VLC_TYPE Coeff_token_vlc [1388]; 
	VLC_TYPE Total_zeros_vlc [7680];
	VLC_TYPE Run_vlc [48];
	VLC_TYPE Run7_vlc [96];
	int Coeff_token_vlc_adress [4];

}   VLC_TABLES ;

typedef struct W_TABLES{
	short W4x4_intra [6][16] ;
	short W4x4_Cb_intra [6][16] ;
	short W4x4_Cr_intra [6][16] ;
	short W4x4_inter [6][16] ; 
	short W4x4_Cb_inter [6][16] ; 
	short W4x4_Cr_inter [6][16] ;
	short W8x8_intra [6][64] ;
	short W8x8_inter [6][64] ;
} W_TABLES;

typedef struct PMbInfo{
	unsigned char name;
    unsigned char type_0;
	unsigned char type_1;
	unsigned char partcount;
    unsigned char Mode;
} PMbInfo;



typedef struct IMbInfo{
    unsigned char type;
    unsigned char pred_mode;
    unsigned char Cbp;
} IMbInfo;



typedef struct {
#ifdef ENABLE_PACKET_LOSS_DETECTION
	int layer_id[MAX_SCALABLE_LAYERS];
	int priority_id[MAX_SCALABLE_LAYERS];
	int discardable_flag[MAX_SCALABLE_LAYERS];
	int dependency_id[MAX_SCALABLE_LAYERS];
	int quality_id[MAX_SCALABLE_LAYERS];
	int temporal_id[MAX_SCALABLE_LAYERS];
	unsigned char sub_pic_layer_flag[MAX_SCALABLE_LAYERS];
	unsigned char sub_region_layer_flag[MAX_SCALABLE_LAYERS];
	unsigned char iroi_division_info_present_flag[MAX_SCALABLE_LAYERS];
	unsigned char profile_level_info_present_flag[MAX_SCALABLE_LAYERS];
	
	unsigned char bitrate_info_present_flag[MAX_SCALABLE_LAYERS];
	unsigned char frm_rate_info_present_flag[MAX_SCALABLE_LAYERS];
	unsigned char frm_size_info_present_flag[MAX_SCALABLE_LAYERS];
	unsigned char layer_dependency_info_present_flag[MAX_SCALABLE_LAYERS];
	unsigned char parameter_sets_info_present_flag[MAX_SCALABLE_LAYERS];
	unsigned char bitstream_restriction_info_present_flag[MAX_SCALABLE_LAYERS];
	unsigned char exact_inter_layer_pred_flag[MAX_SCALABLE_LAYERS];
	unsigned char exact_sample_value_match_flag[MAX_SCALABLE_LAYERS];
	unsigned char layer_conversion_flag[MAX_SCALABLE_LAYERS];
	unsigned char layer_output_flag[MAX_SCALABLE_LAYERS];

	int layer_profile_level_idc[MAX_SCALABLE_LAYERS];
	int avg_bitrate[MAX_SCALABLE_LAYERS];
	int max_bitrate_layer[MAX_SCALABLE_LAYERS];
	int max_bitrate_layer_representation[MAX_SCALABLE_LAYERS];
	int max_bitrate_calc_window[MAX_SCALABLE_LAYERS];
	int constant_frm_rate_idc[MAX_SCALABLE_LAYERS];
	int avg_frm_rate[MAX_SCALABLE_LAYERS];
	int frm_width_in_mbs_minus1[MAX_SCALABLE_LAYERS];
	int frm_height_in_mbs_minus1[MAX_SCALABLE_LAYERS];
	int base_region_layer_id[MAX_SCALABLE_LAYERS];
	
	int dynamic_rect_flag[MAX_SCALABLE_LAYERS];
	int horizontal_offset[MAX_SCALABLE_LAYERS];
	int vertical_offset[MAX_SCALABLE_LAYERS];
	int region_width[MAX_SCALABLE_LAYERS];
	int region_height[MAX_SCALABLE_LAYERS];
	int roi_id[MAX_SCALABLE_LAYERS];
	unsigned char iroi_grid_flag[MAX_SCALABLE_LAYERS];
	int grid_width_in_mbs_minus1[MAX_SCALABLE_LAYERS];
	int grid_height_in_mbs_minus1[MAX_SCALABLE_LAYERS];
	int num_rois_minus1[MAX_SCALABLE_LAYERS];

	int first_mb_in_roi[MAX_SCALABLE_LAYERS][MAX_SCALABLE_LAYERS];
	int roi_width_in_mbs_minus1[MAX_SCALABLE_LAYERS][MAX_SCALABLE_LAYERS];
	int roi_height_in_mbs_minus1[MAX_SCALABLE_LAYERS][MAX_SCALABLE_LAYERS];
	int num_directly_dependent_layers[MAX_SCALABLE_LAYERS];
	int directly_dependent_layer_id_delta_minus1[MAX_SCALABLE_LAYERS][MAX_SCALABLE_LAYERS];
	int layer_dependency_info_src_layer_id_delta[MAX_SCALABLE_LAYERS];
	int num_seq_parameter_set_minus1[MAX_SCALABLE_LAYERS];
	int seq_parameter_set_id_delta[MAX_SCALABLE_LAYERS][32];
	int num_subset_seq_parameter_set_minus1[MAX_SCALABLE_LAYERS];
	int subset_seq_parameter_set_id_delta[MAX_SCALABLE_LAYERS][32];
	
	int num_pic_parameter_set_minus1[MAX_SCALABLE_LAYERS];
	int pic_parameter_set_id_delta[MAX_SCALABLE_LAYERS][256];
	int parameter_sets_info_src_layer_id_delta[MAX_SCALABLE_LAYERS];
	int motion_vectors_over_pic_boundaries_flag[MAX_SCALABLE_LAYERS];
	int max_bytes_per_pic_denom[MAX_SCALABLE_LAYERS];
	int max_bits_per_mb_denom[MAX_SCALABLE_LAYERS];
	int log2_max_mv_length_horizontal[MAX_SCALABLE_LAYERS];
	int log2_max_mv_length_vertical[MAX_SCALABLE_LAYERS];
	int num_reorder_frames[MAX_SCALABLE_LAYERS];
	int max_dec_frame_buffering[MAX_SCALABLE_LAYERS];
	
	int conversion_type_idc[MAX_SCALABLE_LAYERS];
	unsigned char rewriting_info_flag[MAX_SCALABLE_LAYERS][2];
	int rewriting_profile_level_idc[MAX_SCALABLE_LAYERS][2];
	int rewriting_avg_bitrate[MAX_SCALABLE_LAYERS][2];
	int rewriting_max_bitrate[MAX_SCALABLE_LAYERS][2];
	int pr_dependency_id[NUM_LAYERS];
	int pr_num_minus1[NUM_LAYERS];
	int pr_id[NUM_LAYERS][MAX_QUALITY_LEVELS];
	int pr_profile_level_idc[NUM_LAYERS][MAX_QUALITY_LEVELS];
	
	int pr_avg_bitrate[NUM_LAYERS][MAX_QUALITY_LEVELS];
	int pr_max_bitrate[NUM_LAYERS][MAX_QUALITY_LEVELS];
	int priority_id_setting_uri[NUM_LAYERS];
#endif
	int pr_num_dId_minus1;
} SEI;


typedef void    (*interpol_4x4)(unsigned char image_luma [], unsigned char refPicLXl[],	const short PicWidthSamples, const short stride);

typedef void    (*interpol_8x8)(unsigned char image_luma [], unsigned char refPicLXl[],	const short PicWidthSamples, const short stride);


typedef void (*mb4x4_mode_pf)	(unsigned char * predict_sample, const short PicWidthInPix, int Mask, const short locx, const short locy) ;
typedef void (*mb16x16_mode_pf) (unsigned char * predict_sample, const short PicWidthInPix, int Mask);
typedef void (*mbChroma_mode_pf)(unsigned char * predict_sample, const short PicWidthInPix, int Mask);


typedef struct {
	mb4x4_mode_pf		Ptr_mb4x4_mode[9];
	mb16x16_mode_pf		Ptr_mb16x16_mode[4];
	mbChroma_mode_pf	Ptr_mbChroma_mode_pf[4];
	interpol_4x4		Ptr_interpol_4x4[16];
	interpol_8x8		Ptr_interpol_8x8[16];
} STRUCT_PF;



#endif

