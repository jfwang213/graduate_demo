



#include "type.h"
#include "main_data.h"
#include "svc_type.h"






/* 2010-11-05 13:18:09, application decoder_svc, processor PC_H264_plg type=tcpmp */


#include <stdio.h>
#include <stdlib.h>


#define uchar unsigned char
#define ushort unsigned short
#define uint unsigned int



#define prec_synchro int
#define stream unsigned char
#define image_type unsigned char

#include "SvcInterface.h"
























































































































































































void init_svc_vectors(SVC_VECTORS *svc);
void decode_init_vlc(VLC_TABLES *VLc );
void vector_main_init(MAIN_STRUCT_PF *pf);
int readnal_without_start_code(unsigned char* nal,int nal_length,uchar *buffer);
void NextNalDqId(ConfigSVC *Buffer,NAL *NAL,int *SetLayer,int DqIdMax);
void init_nal_struct(NAL *nal,unsigned char NumOfLayer);
void init_mmo(int num_of_layers,MMO *mmo_stru);
void init_slice(SLICE *slice);
void init_pps(PPS *sps);
void init_sps(SPS *sps);
void InitListMmo(LIST_MMO *RefPicListL0);
void init_int(int  *tab);
void getLayer(int *num_layer);
void init_max_layer(int num_layer);
void initialize_layer(int num_layer);
void ResetLayerChanged();
void SetLastDqId(int num_layer);
void SetArgLayer(int num_layer);
void SetSeqMaxTempId(int num_layer);void SetSeqTempId(int num_layer);
void pic_parameter_set(stream *data,uchar *ao_slice_group_id,PPS *pps,SPS *sps,const int NalBytesInNalunit);
void decoderh264_init(const int pic_width,const int pic_height);
void slice_header_svc(int *Layer,const stream *data,SPS *sps_id,PPS *pps_id,int *entropy_coding_flag,W_TABLES *quantif,LIST_MMO *current_pic,SPS *sps,PPS *pps,int *position,SLICE *slice,MMO *mmo,LIST_MMO RefPicListL0[],LIST_MMO RefPicListL1[],NAL *nal,int *end_of_slice,int *ImgToDisplay,int *xsize,int *ysize,int *AddressPic,int *Crop);
void SliceHeaderSkip(const stream *data,SPS *sps_id,PPS *pps_id,int *entropy_coding_flag,W_TABLES *quantif,LIST_MMO *current_pic,SPS *sps,PPS *pps,int *position,SLICE *slice,MMO *mmo,LIST_MMO RefPicListL0[],LIST_MMO RefPicListL1[],NAL *nal,int *end_of_slice,int *ImgToDisplay,int *xsize,int *ysize,int *AddressPic,int *Crop);
void display_order_init(int argc,uchar **argv);
void DisplayOrderSvc(SPS *sps,LIST_MMO *current_pic,NAL *nal,int *address_pic,int *x_size,int *y_size,int *Crop,int *img_to_display,MMO *mmo);
void Display_tcpmp(const int xsize,const int ysize,int edge,unsigned char *Y,unsigned char *U,unsigned char *V,OPENSVCFRAME *CurrFrame);
void NalUnitSVC(stream *data_in,int *nal_unit_type,int *nal_ref_idc,NAL *Nal);
void init_int(int  *tab);
void svc_calculate_dpb(const int total_memory,const int mv_memory,int nb_of_layers,MMO *mmo_struct,SPS *sps);
void sei_rbsp(stream *data,int NalInRbsp,SPS *sps,SEI *Sei);
void seq_parameter_set(stream *data,SPS *sps);
void PrefixNalUnit(stream *data,int *NalinRbsp,NAL *nal,MMO *mmo,SPS *sps,int *DecRefPic,int *EndOfSlice);
void subset_sps(stream *data,int * NalInRbsp,SPS *sps,NAL *nal);
void NalUnitHeader(const stream *data,int *pos,int *do_ref_pic_marking_base,NAL *nal,int *EndOfSlice);
void slice_base_layer_cavlc(const stream *ai_pcData,int * NalInRbsp,const int *ai_piPosition,const SPS *ai_pstSps,PPS *ai_pstPps,const VLC_TABLES *Vlc,uchar *ai_slice_group_id,LIST_MMO *Current_pic,LIST_MMO *RefListl1,NAL *Nal,SLICE *aio_pstSlice,uchar *aio_tiMbToSliceGroupMap,uchar *aio_tiSlice_table,DATA *aio_tstTab_block,RESIDU *picture_residu,int *   aio_piEnd_of_slice,short *mv_io,short *mvl1_io,short *ref_io,short *refl1_io);
void slice_base_layer_cabac(uchar *data,int *position,int *NalBytesInNalunit,SPS *sps,PPS *pps,uchar *ai_slice_group_id,LIST_MMO *Current_pic,LIST_MMO *RefListl1,NAL *Nal,short *mv_cabac_l0,short *mv_cabac_l1,short *ref_cabac_l0,short *ref_cabac_l1,SLICE *slice,uchar *MbToSliceGroupMap,uchar *slice_table,DATA *Tab_block,RESIDU *picture_residu,int *end_of_slice,short *mvl0_io,short *mb_l1_io,short *refl0_io,short *refl1_io);
void Decode_P_avc(const SPS *ai_pstSps,const PPS *ai_pstPps,const SLICE *ai_pstSlice,const uchar *ai_tiSlice_table,const RESIDU *picture_residu,const STRUCT_PF *pf,const LIST_MMO *ai_pstRefPicListL0,const LIST_MMO *ai_pstCurrent_pic,W_TABLES *quantif_tab,NAL *Nal,short *aio_tiMv,short *aio_tiRef,uchar *aio_tucDpb_luma,uchar *aio_tucDpb_Cb,uchar  *aio_tucDpb_Cr,short *Residu_Img,short *Residu_Cb ,short *Residu_Cr);
void Decode_B_avc(SPS *ai_stSps,PPS *ai_stPps ,SLICE *ai_stSlice,uchar *ai_tSlice_table,RESIDU *picture_residu,MAIN_STRUCT_PF *main_vector,LIST_MMO *ai_pRefPicListL0,LIST_MMO *ai_pRefPicListL1,LIST_MMO *ai_pCurrent_pic,W_TABLES *quantif,NAL *Nal,short *aio_tMv_l0,short *aio_tMv_l1,short *aio_tref_l0,short *aio_tref_l1,uchar *aio_tDpb_luma,uchar *aio_tDpb_Cb,uchar *aio_tDpb_Cr,short *Residu_img,short *Residu_Cb ,short *Residu_Cr);
void Decode_I_avc(SPS *sps,PPS *pps,SLICE *slice,uchar *slice_table,RESIDU *picture_residu,STRUCT_PF *pf,W_TABLES *quantif_tab,NAL *Nal,uchar *image,uchar *image_Cb,uchar *image_Cr);
void FinishFrameSVC(const int NbMb,NAL *Nal,SPS *Sps,PPS *Pps,LIST_MMO *Current_pic,SLICE *Slice,int EndOfSlice,uchar *SliceTab,DATA *TabBlbock,RESIDU *Residu,short *MvL0,short *MvL1,short *RefL0,short *RefL1,int *Crop,int *ImgToDisplay,int *AdressPic,MMO *Mmo,unsigned char *RefY,unsigned char *RefU,unsigned char *RefV,int *xsize,int *ysize);
void slice_data_in_scalable_extension_cavlc(const int size_mb,const stream *ai_pcData,int * NalInRbsp,const int *ai_piPosition,const NAL *nal,const SPS *ai_pstSps,PPS *ai_pstPps,const VLC_TABLES *vlc,uchar *ai_slice_group_id,SLICE *aio_pstSlice,uchar *aio_tiMbToSliceGroupMap,uchar *aio_tiSlice_table,DATA *aio_tstTab_block,RESIDU *residu ,int * aio_piEnd_of_slice);
void SliceCabac(const int size_mb,uchar *data,int *position,int *NalBytesInNalunit,const NAL *Nal,SPS *sps,PPS *pps,uchar *ai_slice_group_id,short *mv_cabac_l0,short *mv_cabac_l1,short *ref_cabac_l0,short *ref_cabac_l1,SLICE *slice,uchar *MbToSliceGroupMap,uchar *slice_table,DATA *Tab_block,RESIDU *picture_residu,int *end_of_slice);
void Decode_P_svc(const int size,const SPS *ai_pstSps,const PPS *ai_pstPps,const SLICE *ai_pstSlice,const NAL *nal,const uchar *ai_tiSlice_table,const DATA *ai_tstTab_Block,RESIDU *residu,STRUCT_PF *baseline_vector,const LIST_MMO *ai_pstRefPicListL0,const LIST_MMO *ai_pstCurrent_pic,W_TABLES *quantif_tab,SVC_VECTORS *svc,short *px,short *py,short *Upsampling_tmp,short *xk16,short *xp16,short *yk16,short* yp16,short *aio_tiMv,short *aio_tiRef,uchar *aio_tucDpb_luma,uchar *aio_tucDpb_Cb,uchar  *aio_tucDpb_Cr,short *Residu_Img,short *Residu_Cb ,short *Residu_Cr);
void Decode_B_svc(const int size,const SPS *ai_pstSps,const PPS *ai_pstPps,const SLICE *ai_pstSlice,const NAL *nal,const uchar *ai_tiSlice_table,const DATA *ai_tstTab_Block,RESIDU *residu,MAIN_STRUCT_PF *baseline_vector,const LIST_MMO *ai_pstRefPicListL0,const LIST_MMO *ai_pstRefPicListL1,const LIST_MMO *ai_pstCurrent_pic,W_TABLES *quantif_tab,SVC_VECTORS *svc,short *px,short *py,short *Upsampling_tmp,short *k16,short *p16,short *yk16,short *yp16,short *aio_tiMv_l0,short *aio_tMv_l1,short *aio_tiRef_l0,short *aio_tiRef_l1,uchar *aio_tucDpb_luma,uchar *aio_tucDpb_Cb,uchar  *aio_tucDpb_Cr,short *Residu_Img,short *Residu_Cb ,short *Residu_Cr);
void Decode_I_svc(const int size,SPS *sps,PPS *pps,SLICE *slice,NAL *nal,uchar *slice_table,DATA *Block,RESIDU *residu,STRUCT_PF *vector,LIST_MMO *Current_pic,W_TABLES *quantif,unsigned char *aio_tucImage,unsigned char *aio_tucImage_Cb,unsigned char  *aio_tucImage_Cr);
void Extract_tcpmp(int xsize,int ysize,int edge,int Crop,uchar *img_luma_in,uchar *img_Cb_in,uchar *img_Cr_in,int address_pic,OPENSVCFRAME *Frame);
typedef struct{
  image_type Display_1_Extract_1_Image_Y_o[3279368];
  image_type Display_1_Extract_Image_Y_o[3279368];
  NAL DqIdNextNal_Nal_o[1];
  int DqIdNextNal_set_layer_o[1];
  int GetNalBytes_NalUnitBytes_o_buf[1];
  ConfigSVC GetNalBytes_StreamType[1];
  int GetNalBytes_rbsp_o_size[1];

  stream GetNalBytes_rbsp_o[1013760];
  int NumBytesInNal_buf_1[1];
  short decoder_svc_MvBuffer_1_Mv[6266880];
  short decoder_svc_MvBuffer_1_Ref[3133440];
  short decoder_svc_MvBuffer_Mv[6266880];
  short decoder_svc_MvBuffer_Ref[3133440];
  int decoder_svc_NalUnit_NalRefIdc_io[1];
  int decoder_svc_NalUnit_NalUnitType_io[1];
  int decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_Crop[1];
  int decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_address_pic_o[1];
  int decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_xsize_o[1];
  int decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_ysize_o[1];
  int decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_1_Pos[1];
  int decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos[1];
  int decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_entropy_coding_flag[1];
  PPS decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id[1];
  W_TABLES decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif[1];
  SPS decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id[1];
  int decoder_svc_Nal_Compute_NalDecodingProcess_Slice_type_SliceType_o[1];
  int decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_entropy_coding_flag[1];
  PPS decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_pps_id[1];
  W_TABLES decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_quantif[1];
  SPS decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_sps_id[1];
  short decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_mv_cabac_l0_o[261120];
  short decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_mv_cabac_l1_o[261120];
  short decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_ref_cabac_l0_o[32640];
  short decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_ref_cabac_l1_o[32640];
  int decoder_svc_Nal_Compute_SetPos_Pos[1];
  int decoder_svc_Nal_Compute_SliceHeaderIDR_entropy_coding_flag[1];
  PPS decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id[1];
  W_TABLES decoder_svc_Nal_Compute_SliceHeaderIDR_quantif[1];
  SPS decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id[1];
  int decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o[1];
  SEI decoder_svc_Nal_Compute_sei_rbsp_Sei[1];
  int decoder_svc_Nal_Compute_seq_parameter_set_IdOfsps_o[1];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_Upsampling_tmp[2088960];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_px[1920];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_py[1088];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_xk16[1920];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_xp16[1920];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_yk16[1088];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_yp16[1088];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_Upsampling_tmp[2088960];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_px[1920];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_py[1088];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_xk16[1920];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_xp16[1920];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_yk16[1088];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_yp16[1088];
  int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_Crop[1];
  int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_address_pic_o[1];
  int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_xsize_o[1];
  int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_ysize_o[1];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_mv_cabac_l0_o[261120];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_mv_cabac_l1_o[261120];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_ref_cabac_l0_o[32640];
  short decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_ref_cabac_l1_o[32640];
  int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_type_SliceType_o[1];
  int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_entropy_coding_flag[1];
  PPS decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id[1];
  W_TABLES decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif[1];
  SPS decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id[1];
  int decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_entropy_coding_flag[1];
  PPS decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_pps_id[1];
  W_TABLES decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_quantif[1];
  SPS decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_sps_id[1];
  short decoder_svc_Nal_Compute_slice_layer_main_CABAC_mv_cabac_l0_o[261120];
  short decoder_svc_Nal_Compute_slice_layer_main_CABAC_mv_cabac_l1_o[261120];
  short decoder_svc_Nal_Compute_slice_layer_main_CABAC_ref_cabac_l0_o[32640];
  short decoder_svc_Nal_Compute_slice_layer_main_CABAC_ref_cabac_l1_o[32640];
  uchar decoder_svc_PictureBuffer_RefU[13117440];
  uchar decoder_svc_PictureBuffer_RefV[13117440];
  uchar decoder_svc_PictureBuffer_RefY[52469760];
  short decoder_svc_ResiduBuffer_RefU[2186240];
  short decoder_svc_ResiduBuffer_RefV[2186240];
  short decoder_svc_ResiduBuffer_RefY[8744960];
  DATA decoder_svc_Residu_Block[8160];
  LIST_MMO decoder_svc_Residu_Current_pic[1];
  uchar decoder_svc_Residu_MbToSliceGroupMap[8160];
  MMO decoder_svc_Residu_Mmo[1];
  PPS decoder_svc_Residu_PPS[255];
  LIST_MMO decoder_svc_Residu_RefL0[16];
  LIST_MMO decoder_svc_Residu_RefL1[16];
  RESIDU decoder_svc_Residu_Residu[32640];
  SPS decoder_svc_Residu_SPS[32];
  SLICE decoder_svc_Residu_Slice[1];
  uchar decoder_svc_Residu_SliceGroupId[8160];
  uchar decoder_svc_Residu_SliceTab[8160];
  int decoder_svc_SetZeor_Pos[1];
  SVC_VECTORS decoder_svc_Svc_Vectors_PC_H264_plg_Svc_Vectors[1];
  int decoder_svc_VideoParameter_Crop[1];
  int decoder_svc_VideoParameter_EndOfSlice[1];
  int decoder_svc_VideoParameter_ImgToDisplay[1];
  int decoder_svc_VideoParameter_address_pic_o[1];
  int decoder_svc_VideoParameter_xsize_o[1];
  int decoder_svc_VideoParameter_ysize_o[1];
  VLC_TABLES decoder_svc_VlcTab_PC_H264_plg_o[1];
  MAIN_STRUCT_PF decoder_svc_slice_main_vector_PC_H264_plg_Main_vector_o[1];
  uchar read_SVC_DataFile_o[1013760];
  int read_SVC_pos_o[1];
  SLICE *decoder_svc_Nal_Compute_slice_layer_main_CondO9_o;
  short *decoder_svc_Nal_Compute_slice_layer_main_CondO8_o;
  short *decoder_svc_Nal_Compute_slice_layer_main_CondO7_o;
  short *decoder_svc_Nal_Compute_slice_layer_main_CondO6_o;
  short *decoder_svc_Nal_Compute_slice_layer_main_CondO5_o;
  uchar *decoder_svc_Nal_Compute_slice_layer_main_CondO4_o;
  RESIDU *decoder_svc_Nal_Compute_slice_layer_main_CondO3_o;
  DATA *decoder_svc_Nal_Compute_slice_layer_main_CondO0_o;
  uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o;
  SLICE *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o;
  int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO3_o;
  DATA *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o;
  RESIDU *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o;
  short *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o;
  short *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o;
  short *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o;
  short *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o;
  uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o;
  uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o;
  uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o;
  SLICE *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o;
  short *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o;
  short *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o;
  short *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o;
  short *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o;
  uchar *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o;
  RESIDU *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o;
  int *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO1_o;
  DATA *decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO0_o;
  uchar *decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o;
  uchar *decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o;
  uchar *decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o;
  int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO5_o;
  int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO7_o;
  int *decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO5_o;
  int *decoder_svc_Nal_Compute_NalDecodingProcess_CondO7_o;
  int *decoder_svc_Nal_Compute_CondO7_o;
  int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO4_o;
  int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO6_o;
  int *decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO4_o;
  int *decoder_svc_Nal_Compute_NalDecodingProcess_CondO6_o;
  int *decoder_svc_Nal_Compute_CondO6_o;
  int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO2_o;
  int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO5_o;
  int *decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO2_o;
  int *decoder_svc_Nal_Compute_NalDecodingProcess_CondO5_o;
  int *decoder_svc_Nal_Compute_CondO5_o;
  uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO4_o;
  uchar *decoder_svc_Nal_Compute_NalDecodingProcess_CondO4_o;
  uchar *decoder_svc_Nal_Compute_CondO4_o;
  uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO3_o;
  uchar *decoder_svc_Nal_Compute_NalDecodingProcess_CondO3_o;
  uchar *decoder_svc_Nal_Compute_CondO3_o;
  uchar *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO2_o;
  uchar *decoder_svc_Nal_Compute_NalDecodingProcess_CondO2_o;
  uchar *decoder_svc_Nal_Compute_CondO2_o;
  int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO1_o;
  int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO1_o;
  int *decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO1_o;
  int *decoder_svc_Nal_Compute_NalDecodingProcess_CondO1_o;
  int *decoder_svc_Nal_Compute_CondO1_o;
  int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO0_o;
  int *decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO0_o;
  int *decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO0_o;
  int *decoder_svc_Nal_Compute_NalDecodingProcess_CondO0_o;
  int *decoder_svc_Nal_Compute_CondO0_o;
  int *NumBytesInNal_o_1;
  int *GetNalBytes_NalUnitBytes_o;


}RAM_tcpmp;

int SVCDecoder_init(RAM_tcpmp **Player) { /* for link with C runtime boot */
  RAM_tcpmp *RAM_tcpmp_alloc_ = malloc(sizeof(RAM_tcpmp));
  if (!RAM_tcpmp_alloc_){
      #ifdef TCPMP
         #ifdef WIN_32
 	         MessageBoxA(NULL,(LPCSTR)"The decoder requieres more memory than the architecture can provide", (LPCSTR)"Open  Decoder has encountered an error", MB_OK);
         #endif
      #endif
	 exit(20);
  }
  *Player = RAM_tcpmp_alloc_;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO9_o = RAM_tcpmp_alloc_->decoder_svc_Residu_Slice;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO8_o = RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO7_o = RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO6_o = RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO5_o = RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO4_o = RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO3_o = RAM_tcpmp_alloc_->decoder_svc_Residu_Residu;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO0_o = RAM_tcpmp_alloc_->decoder_svc_Residu_Block;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o = RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o = RAM_tcpmp_alloc_->decoder_svc_Residu_Slice;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO3_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o = RAM_tcpmp_alloc_->decoder_svc_Residu_Block;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o = RAM_tcpmp_alloc_->decoder_svc_Residu_Residu;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o = RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o = RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o = RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o = RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o = RAM_tcpmp_alloc_->decoder_svc_Residu_Slice;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o = RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o = RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o = RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o = RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o = RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o = RAM_tcpmp_alloc_->decoder_svc_Residu_Residu;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO1_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO0_o = RAM_tcpmp_alloc_->decoder_svc_Residu_Block;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO5_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO7_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO5_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO7_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO7_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO4_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO6_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO4_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO6_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO6_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO2_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO5_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO2_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO5_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO5_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO4_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO4_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO4_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO3_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO3_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO3_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO2_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO2_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO2_o = RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO1_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO1_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO1_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO1_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO1_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO0_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO0_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO0_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO0_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
  RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO0_o = RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
  RAM_tcpmp_alloc_->NumBytesInNal_o_1 = RAM_tcpmp_alloc_->NumBytesInNal_buf_1;
  RAM_tcpmp_alloc_->GetNalBytes_NalUnitBytes_o = RAM_tcpmp_alloc_->GetNalBytes_NalUnitBytes_o_buf;

  init_svc_vectors(RAM_tcpmp_alloc_->decoder_svc_Svc_Vectors_PC_H264_plg_Svc_Vectors);
  decode_init_vlc(RAM_tcpmp_alloc_->decoder_svc_VlcTab_PC_H264_plg_o);
  vector_main_init(RAM_tcpmp_alloc_->decoder_svc_slice_main_vector_PC_H264_plg_Main_vector_o);
  
  
  init_nal_struct(RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, 4);
  init_int(RAM_tcpmp_alloc_->decoder_svc_SetZeor_Pos);
  init_slice(RAM_tcpmp_alloc_->decoder_svc_Residu_Slice);
  init_sps(RAM_tcpmp_alloc_->decoder_svc_Residu_SPS);
  init_pps(RAM_tcpmp_alloc_->decoder_svc_Residu_PPS);
  InitListMmo(RAM_tcpmp_alloc_->decoder_svc_Residu_RefL0);
  InitListMmo(RAM_tcpmp_alloc_->decoder_svc_Residu_RefL1);
  InitListMmo(RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic);
  init_mmo(4, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo);
  init_int(RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice);
  init_int(RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay);
  init_int(RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o);
  init_int(RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o);
  init_int(RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o);
  init_int(RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop);
  init_max_layer(4);
  initialize_layer(0);
  init_mmo(4, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo);
  ResetLayerChanged();
  SetLastDqId(0);
  SetArgLayer(0);
  SetSeqMaxTempId(255);
  SetSeqTempId(255);
  
  
  decoderh264_init(1920, 1088);
  
  
  
  
  
  
  
  
  
  RAM_tcpmp_alloc_->NumBytesInNal_o_1[0]=1013760;
  
  
  
  return 1;

  }

  int decodeNAL(RAM_tcpmp *RAM_tcpmp_alloc_, unsigned char* nal, int nal_length, OPENSVCFRAME *CurrFrame, int DqIdMax){
    int result = 0;
    
    RAM_tcpmp_alloc_->GetNalBytes_rbsp_o_size[0] = readnal_without_start_code(nal, nal_length, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o);
    NextNalDqId(RAM_tcpmp_alloc_->GetNalBytes_StreamType, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->DqIdNextNal_set_layer_o, DqIdMax);
    
    
    
    NalUnitSVC(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->decoder_svc_NalUnit_NalUnitType_io, RAM_tcpmp_alloc_->decoder_svc_NalUnit_NalRefIdc_io, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o);
    init_int(RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay);
    getLayer(RAM_tcpmp_alloc_->DqIdNextNal_set_layer_o);
    switch ( RAM_tcpmp_alloc_->decoder_svc_NalUnit_NalUnitType_io[0]) { /* switch_3 */
      case 5 : {/* case_4 */
        svc_calculate_dpb((2186240 * (5 + 16 + 4 - 1)), (1920 * 1088 / 8 * 4 * (5 + 1)), 4, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS);
      break; }/* case_4 */
      case 6 : {/* case_5 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
        sei_rbsp(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o_size[0], RAM_tcpmp_alloc_->decoder_svc_Residu_SPS, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_sei_rbsp_Sei);
      break; }/* case_5 */
      case 7 : {/* case_6 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
        seq_parameter_set(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS);
      break; }/* case_6 */
      case 8 : {/* case_7 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
      break; }/* case_7 */
      case 14 : {/* case_8 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
        PrefixNalUnit(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o_size, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS, RAM_tcpmp_alloc_->decoder_svc_SetZeor_Pos, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice);
      break; }/* case_8 */
      case 15 : {/* case_9 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
        subset_sps(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o_size, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o);
      break; }/* case_9 */
      case 20 : {/* case_10 */
        NalUnitHeader(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, RAM_tcpmp_alloc_->decoder_svc_SetZeor_Pos, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice);
      break; }/* case_10 */
    } /* end switch_3 */
    
    
    switch ( RAM_tcpmp_alloc_->decoder_svc_NalUnit_NalUnitType_io[0]) { /* switch_11 */
      case 1 : {/* case_12 */
        switch ( RAM_tcpmp_alloc_->decoder_svc_SetZeor_Pos[0]) { /* switch_13 */
          case 0 : {/* case_14 */
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos[0] = 8;
            slice_header_svc(RAM_tcpmp_alloc_->DqIdNextNal_set_layer_o, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_entropy_coding_flag, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS, RAM_tcpmp_alloc_->decoder_svc_Residu_PPS, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos, RAM_tcpmp_alloc_->decoder_svc_Residu_Slice, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL0, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL1, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop);
          break; }/* case_14 */
          case 3 : {/* case_15 */
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_1_Pos[0] = 8;
            SliceHeaderSkip(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_pps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_entropy_coding_flag, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_quantif, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS, RAM_tcpmp_alloc_->decoder_svc_Residu_PPS, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_1_Pos, RAM_tcpmp_alloc_->decoder_svc_Residu_Slice, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL0, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL1, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop);
            if (RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice[0]==1) { /* if_16 */
              RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
              RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
              RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
              RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
              RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
            } /* end if_16 */
          break; }/* case_15 */
        } /* end switch_13 */
      break; }/* case_12 */
      case 5 : {/* case_17 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SetPos_Pos[0] = 8;
        slice_header_svc(RAM_tcpmp_alloc_->DqIdNextNal_set_layer_o, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_entropy_coding_flag, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_quantif, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS, RAM_tcpmp_alloc_->decoder_svc_Residu_PPS, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SetPos_Pos, RAM_tcpmp_alloc_->decoder_svc_Residu_Slice, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL0, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL1, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop);
        switch ( RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_entropy_coding_flag[0]) { /* switch_18 */
          case 0 : {/* case_19 */
            slice_base_layer_cavlc(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o_size, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SetPos_Pos, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, RAM_tcpmp_alloc_->decoder_svc_VlcTab_PC_H264_plg_o, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceGroupId, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL1, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Residu_Slice, RAM_tcpmp_alloc_->decoder_svc_Residu_MbToSliceGroupMap, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab, RAM_tcpmp_alloc_->decoder_svc_Residu_Block, RAM_tcpmp_alloc_->decoder_svc_Residu_Residu, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref);
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO9_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Slice;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO8_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Residu;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Block;
          break; }/* case_19 */
          case 1 : {/* case_20 */
            slice_base_layer_cabac(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o_size, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SetPos_Pos, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceGroupId, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL1, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CABAC_mv_cabac_l0_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CABAC_mv_cabac_l1_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CABAC_ref_cabac_l0_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CABAC_ref_cabac_l1_o, RAM_tcpmp_alloc_->decoder_svc_Residu_Slice, RAM_tcpmp_alloc_->decoder_svc_Residu_MbToSliceGroupMap, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab, RAM_tcpmp_alloc_->decoder_svc_Residu_Block, RAM_tcpmp_alloc_->decoder_svc_Residu_Residu, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref);
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO9_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Slice;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO8_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Residu;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Block;
          break; }/* case_20 */
        } /* end switch_18 */
      break; }/* case_17 */
      case 8 : {/* case_21 */
        pic_parameter_set(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceGroupId, RAM_tcpmp_alloc_->decoder_svc_Residu_PPS, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o_size[0]);
      break; }/* case_21 */
      case 20 : {/* case_22 */
        switch ( RAM_tcpmp_alloc_->decoder_svc_SetZeor_Pos[0]) { /* switch_23 */
          case 0 : {/* case_24 */
            slice_header_svc(RAM_tcpmp_alloc_->DqIdNextNal_set_layer_o, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_entropy_coding_flag, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS, RAM_tcpmp_alloc_->decoder_svc_Residu_PPS, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, RAM_tcpmp_alloc_->decoder_svc_Residu_Slice, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL0, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL1, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop);
          break; }/* case_24 */
          case 3 : {/* case_25 */
            SliceHeaderSkip(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_pps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_entropy_coding_flag, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_quantif, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS, RAM_tcpmp_alloc_->decoder_svc_Residu_PPS, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, RAM_tcpmp_alloc_->decoder_svc_Residu_Slice, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL0, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL1, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop);
            switch ( RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice[0]) { /* switch_26 */
              case 0 : {/* case_27 */
                DisplayOrderSvc(RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_svc_sps_id, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_address_pic_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_xsize_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_ysize_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_Crop, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo);
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_ysize_o;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_xsize_o;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_address_pic_o;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_DisplaySvc_Crop;
              break; }/* case_27 */
              case 1 : {/* case_28 */
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
              break; }/* case_28 */
            } /* end switch_26 */
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO5_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO4_o;
          break; }/* case_25 */
        } /* end switch_23 */
      break; }/* case_22 */
    } /* end switch_11 */
    
    
    switch ( RAM_tcpmp_alloc_->decoder_svc_NalUnit_NalUnitType_io[0]) { /* switch_29 */
      case 1 : {/* case_30 */
        switch ( RAM_tcpmp_alloc_->decoder_svc_SetZeor_Pos[0]) { /* switch_31 */
          case 0 : {/* case_32 */
            switch ( RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_entropy_coding_flag[0]) { /* switch_33 */
              case 0 : {/* case_34 */
                slice_base_layer_cavlc(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o_size, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, RAM_tcpmp_alloc_->decoder_svc_VlcTab_PC_H264_plg_o, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceGroupId, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL1, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Residu_Slice, RAM_tcpmp_alloc_->decoder_svc_Residu_MbToSliceGroupMap, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab, RAM_tcpmp_alloc_->decoder_svc_Residu_Block, RAM_tcpmp_alloc_->decoder_svc_Residu_Residu, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref);
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Slice;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Residu;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Block;
              break; }/* case_34 */
              case 1 : {/* case_35 */
                slice_base_layer_cabac(RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o_size, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Set_Pos_Pos, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceGroupId, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL1, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_mv_cabac_l0_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_mv_cabac_l1_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_ref_cabac_l0_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CABAC_ref_cabac_l1_o, RAM_tcpmp_alloc_->decoder_svc_Residu_Slice, RAM_tcpmp_alloc_->decoder_svc_Residu_MbToSliceGroupMap, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab, RAM_tcpmp_alloc_->decoder_svc_Residu_Block, RAM_tcpmp_alloc_->decoder_svc_Residu_Residu, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref);
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Slice;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Residu;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Block;
              break; }/* case_35 */
            } /* end switch_33 */
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Slice_type_SliceType_o[0]=RAM_tcpmp_alloc_->decoder_svc_Residu_Slice[0].slice_type;
            switch ( RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Slice_type_SliceType_o[0]) { /* switch_36 */
              case 0 : {/* case_37 */
                Decode_P_avc(RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o, RAM_tcpmp_alloc_->decoder_svc_slice_main_vector_PC_H264_plg_Main_vector_o->baseline_vectors, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL0, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, &(RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o[RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic->MvMemoryAddress]), &(RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o[RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic->MvMemoryAddress >> 1]), RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV, RAM_tcpmp_alloc_->decoder_svc_ResiduBuffer_RefY, RAM_tcpmp_alloc_->decoder_svc_ResiduBuffer_RefU, RAM_tcpmp_alloc_->decoder_svc_ResiduBuffer_RefV);
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
              break; }/* case_37 */
              case 1 : {/* case_38 */
                Decode_B_avc(RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o, RAM_tcpmp_alloc_->decoder_svc_slice_main_vector_PC_H264_plg_Main_vector_o, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL0, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL1, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV, RAM_tcpmp_alloc_->decoder_svc_ResiduBuffer_RefY, RAM_tcpmp_alloc_->decoder_svc_ResiduBuffer_RefU, RAM_tcpmp_alloc_->decoder_svc_ResiduBuffer_RefV);
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
              break; }/* case_38 */
              case 2 : {/* case_39 */
                Decode_I_avc(RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o, RAM_tcpmp_alloc_->decoder_svc_slice_main_vector_PC_H264_plg_Main_vector_o->baseline_vectors, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_quantif, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, &(RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY[RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic->MemoryAddress]), &(RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU[RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic->MemoryAddress>>2]), &(RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV[RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic->MemoryAddress>>2]));
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
              break; }/* case_39 */
            } /* end switch_36 */
            FinishFrameSVC(1920*1088/256, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_SliceHeader_pps_id, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO9_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO1_o[0], RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO4_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO0_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO3_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO5_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO6_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO7_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_layer_CondO8_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o);
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO2_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO1_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Decode_IPB_avc_CondO0_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
          break; }/* case_32 */
          case 3 : {/* case_40 */
            if (RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice[0]==0) { /* if_41 */
              DisplayOrderSvc(RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_slice_header_svc_sps_id, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_address_pic_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_xsize_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_ysize_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_Crop, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo);
              RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_ysize_o;
              RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_xsize_o;
              RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_address_pic_o;
              RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
              RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_DisplaySvc_Crop;
            } /* end if_41 */
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO5_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO4_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO2_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO1_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_Display_CondO0_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
          break; }/* case_40 */
        } /* end switch_31 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO7_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO6_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO5_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO4_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO3_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO2_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO1_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_NalDecodingProcess_CondO0_o;
      break; }/* case_30 */
      case 5 : {/* case_42 */
        Decode_I_avc(RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO9_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO4_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO3_o, RAM_tcpmp_alloc_->decoder_svc_slice_main_vector_PC_H264_plg_Main_vector_o->baseline_vectors, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_quantif, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, &(RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY[RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic->MemoryAddress]), &(RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU[RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic->MemoryAddress>>2]), &(RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV[RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic->MemoryAddress>>2]));
        FinishFrameSVC(1920*1088/256, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_SliceHeaderIDR_pps_id, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Residu_Slice, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice[0], RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO4_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO0_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO3_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO5_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO6_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO7_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_main_CondO8_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o);
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
      break; }/* case_42 */
      case 6 : {/* case_43 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
      break; }/* case_43 */
      case 7 : {/* case_44 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
      break; }/* case_44 */
      case 8 : {/* case_45 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
      break; }/* case_45 */
      case 14 : {/* case_46 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
      break; }/* case_46 */
      case 15 : {/* case_47 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
      break; }/* case_47 */
      case 20 : {/* case_48 */
        switch ( RAM_tcpmp_alloc_->decoder_svc_SetZeor_Pos[0]) { /* switch_49 */
          case 0 : {/* case_50 */
            switch ( RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_entropy_coding_flag[0]) { /* switch_51 */
              case 0 : {/* case_52 */
                slice_data_in_scalable_extension_cavlc(1920*1088/256, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o_size, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id, RAM_tcpmp_alloc_->decoder_svc_VlcTab_PC_H264_plg_o, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceGroupId, RAM_tcpmp_alloc_->decoder_svc_Residu_Slice, RAM_tcpmp_alloc_->decoder_svc_Residu_MbToSliceGroupMap, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab, RAM_tcpmp_alloc_->decoder_svc_Residu_Block, RAM_tcpmp_alloc_->decoder_svc_Residu_Residu, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice);
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Slice;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Block;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Residu;
              break; }/* case_52 */
              case 1 : {/* case_53 */
                SliceCabac(1920*1088/256, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o, RAM_tcpmp_alloc_->GetNalBytes_rbsp_o_size, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_nal_unit_header_svc_ext_20_pos_o, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceGroupId, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_mv_cabac_l0_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_mv_cabac_l1_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_ref_cabac_l0_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_SliceLayerCabac_ref_cabac_l1_o, RAM_tcpmp_alloc_->decoder_svc_Residu_Slice, RAM_tcpmp_alloc_->decoder_svc_Residu_MbToSliceGroupMap, RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab, RAM_tcpmp_alloc_->decoder_svc_Residu_Block, RAM_tcpmp_alloc_->decoder_svc_Residu_Residu, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice);
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_Residu_SliceTab;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Slice;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_EndOfSlice;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Block;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_Residu_Residu;
              break; }/* case_53 */
            } /* end switch_51 */
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_type_SliceType_o[0]=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o[0].slice_type;
            switch ( RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_type_SliceType_o[0]) { /* switch_54 */
              case 0 : {/* case_55 */
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv;
                Decode_P_svc(1920*1088/256, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS, RAM_tcpmp_alloc_->decoder_svc_Residu_PPS, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, RAM_tcpmp_alloc_->decoder_svc_slice_main_vector_PC_H264_plg_Main_vector_o->baseline_vectors, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL0, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, RAM_tcpmp_alloc_->decoder_svc_Svc_Vectors_PC_H264_plg_Svc_Vectors, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_px, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_py, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_Upsampling_tmp, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_xk16, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_xp16, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_yk16, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_P_svc_yp16, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV, RAM_tcpmp_alloc_->decoder_svc_ResiduBuffer_RefY, RAM_tcpmp_alloc_->decoder_svc_ResiduBuffer_RefU, RAM_tcpmp_alloc_->decoder_svc_ResiduBuffer_RefV);
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
              break; }/* case_55 */
              case 1 : {/* case_56 */
                Decode_B_svc(1920*1088/256, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS, RAM_tcpmp_alloc_->decoder_svc_Residu_PPS, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, RAM_tcpmp_alloc_->decoder_svc_slice_main_vector_PC_H264_plg_Main_vector_o, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL0, RAM_tcpmp_alloc_->decoder_svc_Residu_RefL1, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, RAM_tcpmp_alloc_->decoder_svc_Svc_Vectors_PC_H264_plg_Svc_Vectors, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_px, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_py, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_Upsampling_tmp, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_xk16, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_xp16, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_yk16, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_Decode_B_svc_yp16, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref, RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV, RAM_tcpmp_alloc_->decoder_svc_ResiduBuffer_RefY, RAM_tcpmp_alloc_->decoder_svc_ResiduBuffer_RefU, RAM_tcpmp_alloc_->decoder_svc_ResiduBuffer_RefV);
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
              break; }/* case_56 */
              case 2 : {/* case_57 */
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Ref;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Ref;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_1_Mv;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_MvBuffer_Mv;
                Decode_I_svc(1920*1088/256, RAM_tcpmp_alloc_->decoder_svc_Residu_SPS, RAM_tcpmp_alloc_->decoder_svc_Residu_PPS, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, RAM_tcpmp_alloc_->decoder_svc_slice_main_vector_PC_H264_plg_Main_vector_o->baseline_vectors, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_quantif, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU, RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV);
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
                RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
              break; }/* case_57 */
            } /* end switch_54 */
            FinishFrameSVC(1920*1088/256, RAM_tcpmp_alloc_->DqIdNextNal_Nal_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_sps_id, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_slice_header_21_pps_id, RAM_tcpmp_alloc_->decoder_svc_Residu_Current_pic, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO4_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO3_o[0], RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO5_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO2_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Slice_Layer_CondO1_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO3_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO6_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO4_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO5_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o, RAM_tcpmp_alloc_->decoder_svc_Residu_Mmo, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o, RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o);
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ysize_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_xsize_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_address_pic_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO2_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO1_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Decode_IPB_svc_CondO0_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_ImgToDisplay;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_VideoParameter_Crop;
          break; }/* case_50 */
          case 3 : {/* case_58 */
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO2_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO1_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_Display_CondO0_o;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefY;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefV;
            RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_PictureBuffer_RefU;
          break; }/* case_58 */
        } /* end switch_49 */
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO7_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO7_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO6_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO6_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO5_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO5_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO4_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO4_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO3_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO3_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO2_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO2_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO1_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO1_o;
        RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO0_o=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_slice_layer_in_scalable_ext_20_CondO0_o;
      break; }/* case_48 */
    } /* end switch_29 */
    switch ( RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO1_o[0]) { /* switch_59 */
      case 1 : {/* case_60 */
        {
           Extract_tcpmp((RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO6_o[0]+16*2), (RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO7_o[0]+16*2), 16, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO0_o[0], RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO4_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO2_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO3_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO5_o[0], CurrFrame);
        }
      break; }/* case_60 */
      case 2 : {/* case_61 */
        {
           Extract_tcpmp((RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO6_o[0]+16*2), (RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO7_o[0]+16*2), 16, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO0_o[0], RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO4_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO2_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO3_o, RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO5_o[0], CurrFrame);
        }
      break; }/* case_61 */
    } /* end switch_59 */
    
    {int *NumBytesInNal_o_1_temp = RAM_tcpmp_alloc_->NumBytesInNal_o_1;
    RAM_tcpmp_alloc_->NumBytesInNal_o_1=RAM_tcpmp_alloc_->GetNalBytes_NalUnitBytes_o;
    RAM_tcpmp_alloc_->GetNalBytes_NalUnitBytes_o=NumBytesInNal_o_1_temp;}
    result=RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO1_o[0];
    switch ( RAM_tcpmp_alloc_->decoder_svc_Nal_Compute_CondO1_o[0]) { /* switch_62 */
      case 1 : {/* case_63 */
        {
           int XDIM = ((unsigned int *) RAM_tcpmp_alloc_->Display_1_Extract_Image_Y_o)[0];
           int YDIM = ((unsigned int *) RAM_tcpmp_alloc_->Display_1_Extract_Image_Y_o)[1];
           uchar *Y = RAM_tcpmp_alloc_->Display_1_Extract_Image_Y_o + 8;
           uchar *U = Y + (XDIM + 32) * YDIM;
           uchar *V = U + (XDIM + 32) * YDIM/4 ;
           Display_tcpmp(XDIM, YDIM, 16, Y, U, V, CurrFrame);
        }

      break; }/* case_63 */
      case 2 : {/* case_64 */
        {
           int XDIM = ((unsigned int *) RAM_tcpmp_alloc_->Display_1_Extract_1_Image_Y_o)[0];
           int YDIM = ((unsigned int *) RAM_tcpmp_alloc_->Display_1_Extract_1_Image_Y_o)[1];
           uchar *Y = RAM_tcpmp_alloc_->Display_1_Extract_1_Image_Y_o + 8;
           uchar *U = Y + (XDIM + 32) * YDIM;
           uchar *V = U + (XDIM + 32) * YDIM/4 ;
           Display_tcpmp(XDIM, YDIM, 16, Y, U, V, CurrFrame);
        }

      break; }/* case_64 */
    } /* end switch_62 */
    return result;

  }
  int SVCDecoder_close(RAM_tcpmp *RAM_tcpmp_alloc_){ 
  free(RAM_tcpmp_alloc_);
  return 0;
  
  
  
  
  
  
  
  
  
  
  


} /* end of main */


