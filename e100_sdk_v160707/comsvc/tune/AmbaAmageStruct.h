#ifndef _AMAGE_typedef_struct_H
#define _AMAGE_typedef_struct_H

/* Define AMAMGE */
#define AMAGE_PC_TO_ARM 0
#define AMAGE_ARM_TO_PC 1
#define MAX_AMGAGE_DATA_SIZE 36288

typedef enum _AMAGE_ITEM_ID_s_
{
    //ColorCorrection='B',
 	Adj_VideoLiso='A',
 	Adj_Video_Hiso='B',
 	Adj_Still_Liso='C',
 	Adj_Still_Hiso='D',
 	Adj_Scene='E',
 	Adj_Photo='F',
 	Adj_Param='G',
 	Adj_StillParam='H',
 	Adj_ImageParam='I',
 	Adj_DestillParam='J',
 	Adj_DevideoParam='K',
 	DefaultParam ='L',

    ConfigAAAControl='U',
    TileConfiguration='V',
    ColorCorrection='X',
    AFStatisticSetupEx='Y',
    ExposureControl='Z',

    CHECK_IP ='a',
    GET_RAW ='b',
    DSP_DUMP ='c',
    BW_MODE ='d',
    BP_CALI ='e',
    LENS_CALI ='f',
    AWB_CALI ='g',
    DZOOM='j',
    WARP='k',
    CA='l',
    FPN='m',
    VIGNETTE='n',
    SYSTEM='o',
    RAWDATA='p',
    TUNING_MODE='q',
    SAVE_JPG ='r',
    ITUNER_FILE = 's',
    BIN_PKG = 't',
    FPNCorrection='u',
 GMVSETTING='v',
	HDR_Mode='w',

}AMAGE_ITEM_ID;    
    
typedef enum _AMAGE_TUNING_MODE_
{
    VIDEO = 0,
    STILL = 1,
    IQTable = 2
    
}AMAGE_TUNING_MODE;

typedef struct _JPG_INFO_t{
    void*  addr;
    UINT32 size;
} JPG_INFO_t;

typedef struct blc_level_t_
{
     int r_offset;
     int gr_offset;
     int gb_offset;
     int b_offset;
} blc_level_t;
typedef struct BLC_
{
     INT16 r_black;
     INT16 g_r_black;
     INT16 g_b_black;
     INT16 b_black;
     UINT8 def_method;
} BLC;
typedef struct DGL_
{
      UINT32 level_red;
      UINT32 level_green_even;
      UINT32 level_green_odd;
      UINT32 level_blue;
} DGL;
typedef struct ALS_
{
      UINT8 enable;
    
} ALS;
typedef struct Tone_
{
      UINT16 curve_red[256];
      UINT16 curve_green[256];
      UINT16 curve_blue[256];
} Tone;
typedef struct CS_
{
      UINT8 enable;
      UINT8 Reserved;
      UINT16 Reserved1;
      UINT16 gain_curve[128];
} CS;

typedef struct R2Y_
{
       INT16 m11;
       INT16 m12;
       INT16 m13;
       INT16 m21;
       INT16 m22;
       INT16 m23;
       INT16 m31;
       INT16 m32;
       INT16 m33;
       INT16 y_offset;
       INT16 u_offset;
       INT16 v_offset;
} R2Y;
// added by htchiu on 2013/10/31  
typedef struct WB_
{
       UINT32 r_gain;
       UINT32 g_gain;
       UINT32 b_gain;
}WB;
 typedef struct AMAGE_DSP_IMG_WB_GAIN_s_
{
       UINT32 GainR;
       UINT32 GainG;
       UINT32 GainB;
       UINT32 AeGain;
       UINT32 GlobalDGain;
} AMAGE_DSP_IMG_WB_GAIN_s;
   
typedef struct LE_
{
       UINT8 enable;
       UINT8 radius;
       UINT8 luma_weight_red;
       UINT8 luma_weight[3];  // RGB, added by htchiu on 2013/10/31
       UINT16 gain_curve_table[256];
} LE;

    /// <summary>
    /// noise
    /// </summary>
typedef struct BPC_
{
      UINT8 enable;
      UINT8 hot_pixel_detection_strength;
      UINT8 dark_pixel_detection_strength;
      UINT8 correction_method;
} BPC;
typedef struct LKG_
{
      UINT32 enable;
      INT8 alpha_rr;
      INT8 alpha_rb;
      INT8 alpha_br;
      INT8 alpha_bb;
      UINT16 saturation_level;
} LKG;
typedef struct CNF_
{
      UINT8 enable;
      UINT8 noise_level_cb;    // Cb/Cr, 0-255
      UINT8 noise_level_cr;    // Cb/Cr, 0-255
      UINT16 original_blend_strength_cb; // Cb/Cr, 0-256
      UINT16 original_blend_strength_cr; // Cb/Cr, 0-256
      UINT16 radius;                // 32,64,128
      UINT8 Reserved;
      UINT16 Reserved1;
} CNF;

typedef struct CFA_
{
      UINT8 enable;
     // added by htchiu on 2013/10/31
      UINT16 noise_level[3];   //RGB 0-8192
      UINT16 original_blend_strength[3];  //RGB 0-256
      UINT16 extent_regular[3]; //RGB 0-256
      UINT16 extent_fine[3]; //RGB 0-256
      UINT16 strength_fine[3];
      UINT16 selectivity_regular;   // 0-256
      UINT16 selectivity_fine;      // 0-256
} CFA;

typedef struct CMF_
{
      int enable;
      UINT16 cb_adaptive_strength;
      UINT16 cr_adaptive_strength;
      UINT8 cb_non_adaptive_strength;
      UINT8 cr_non_adaptive_strength;
      UINT16 cb_adaptive_amount;
      UINT16 cr_adaptive_amount;
      UINT16 Reserved;
} CMF;
typedef struct sharpen_level_t_
{
      UINT8 low;
      UINT8 low_delta;
      UINT8 low_strength;
      UINT8 mid_strength;
      UINT8 high;
      UINT8 high_delta;
      UINT8 high_strength;
      UINT8 method;
} sharpen_level_t;
typedef struct ASF_
{
      UINT16 iso_strength;                   // 0-256
      UINT16 dir_strength;                   // 0-256
      UINT8 directional_decide_t0;     // 0-255
      UINT8 directional_decide_t1;     // 0-255
      UINT16 adaptation;                     // 0-256
      UINT8 max_change;                    // 0-255
      sharpen_level_t level_adapt;         // 16 unit and range 0-255 for low/mid/high strength
      sharpen_level_t level_str_adjust; // 32 unit and range 0-64 for low/mid/high strength
} ASF;
typedef struct FIR_
{
      UINT8 Specify;
      UINT16 PerDirFirIsoStrengths[9];
      UINT16 PerDirFirDirStrengths[9];
      UINT16 PerDirFirDirAmounts[9];
      UINT16 Coefs[225];//9*25
      UINT16 StrengthIso;
      UINT16 StrengthDir;
      UINT16 EdgeThresh;
      UINT8 WideEdgeDetect;
} FIR;
typedef struct CORING_
{
      UINT8 coring[256];
} CORING;
typedef struct MAX_CHNAGE_
{
      UINT8 max_change_up;
      UINT8 max_change_down;
} MAX_CHNAGE;
typedef struct LUMA_HIGH_FREQ_NR_
{
      INT16 strength_coarse;
      INT16 strength_fine;
} LUMA_HIGH_FREQ_NR;
typedef struct CDNR_
{
      int enable;
      int strength;
} CDNR;
typedef struct GMV_
{
      UINT8 enable;
      INT16 mv_x;
      INT16 mv_y;
} GMV;
typedef struct AAA_CTRL_
{
      UINT8 awb_op;
      UINT8 ae_op;
      UINT8 af_op;
      UINT8 adj_op;
} AAA_CTRL;
 
typedef struct video_mctf_one_chan_t_
{
       UINT8 temporal_alpha_0;
       UINT8 temporal_alpha_1;
       UINT8 temporal_alpha_2;
       UINT8 temporal_alpha_3;
       UINT8 temporal_t0;
       UINT8 temporal_t1;
       UINT8 temporal_t2;
       UINT8 temporal_t3;
       UINT8 temporal_maxchange;
       UINT16 radius;        //0-256
       UINT16 str_3d;        //0-256
       UINT16 str_spatial;   //0-256
       UINT16 level_adjust;  //0-256
} video_mctf_one_chan_t;

typedef struct AMBA_DSP_IMG_VIDEO_MCTF_MB_TEMPORAL_t_
{
        UINT8 mb_enable;
        UINT8 mb_temporal_motion_detection_delay;
        UINT8 mb_temporal_frames_combine_num;
        UINT8 mb_temporal_frames_combine_thresh;
        UINT8 mb_temporal_frames_combine_min_above_thresh;
        UINT8 mb_temporal_frames_combine_val_below_thresh;
        UINT8 mb_temporal_max1;
        UINT8 mb_temporal_min1;
        UINT8 mb_temporal_max2;
        UINT8 mb_temporal_min2;

        UINT16 mb_temporal_mul;
        UINT16 mb_temporal_sub;
        UINT8 mb_temporal_shift;
}AMBA_DSP_IMG_VIDEO_MCTF_MB_TEMPORAL_t;
typedef struct MCTF_
{
        UINT8 enable;
        video_mctf_one_chan_t chan_info[3];
        UINT16 y_combined_strength; //0-256
        AMBA_DSP_IMG_VIDEO_MCTF_MB_TEMPORAL_t MbTemporal;
        UINT8 Reserved;
} MCTF;

typedef struct _AMBA_DSP_IMG_LEVEL_t_ {
    UINT8   Low;
    UINT8   LowDelta;
    UINT8   LowStrength;
    UINT8   MidStrength;
    UINT8   High;
    UINT8   HighDelta;
    UINT8   HighStrength;
    UINT8   Method;
} AMBA_DSP_IMG_LEVEL_t;


typedef struct _AMBA_DSP_IMG_VIDEO_MCTF_LEVEL_ONE_CHAN_t_ {
        INT32                                       TemporalMaxChangeNotT0T1LevelBased;
        AMBA_DSP_IMG_LEVEL_t     TemporalEitherMaxChangeOrT0T1Add;
} AMBA_DSP_IMG_VIDEO_MCTF_LEVEL_ONE_CHAN_t;

typedef struct _AMBA_DSP_IMG_VIDEO_MCTF_LEVEL_t_ {
    AMBA_DSP_IMG_VIDEO_MCTF_LEVEL_ONE_CHAN_t    ChanLevel[3];    /* YCbCr */
} AMBA_DSP_IMG_VIDEO_MCTF_LEVEL_t;


typedef struct MCTF_PKG_t
{
    MCTF mctf;
    AMBA_DSP_IMG_VIDEO_MCTF_LEVEL_t mctf_level;
    
}MCTF_PKG;
typedef struct MISMATCH_INFO_
{
         UINT8 narrow_enable;
         UINT8 wide_enable;
         UINT16 wide_safety;
         UINT16 wide_thresh;
} MISMATCH_INFO;

typedef struct DEMOSAIC_INFO_
{
         UINT16 activity_thresh;
         UINT16 activity_difference_thresh;
         UINT16 grad_clip_thresh;
         UINT16 grad_noise_thresh;
         UINT16 zipper_noise_add_thresh;
         UINT16 zipper_noise_mult_thresh;
         UINT16 black_white_resolution_detail;
         UINT16 clamp_directional_interpolation;
} DEMOSAIC_INFO;
    
    
typedef struct AMAGE_DSP_IMG_MAX_CHANGE_s_
{
         UINT8   Up5x5;
         UINT8   Down5x5;
         UINT8   Up;
         UINT8   Down;
} AMAGE_DSP_IMG_MAX_CHANGE_s;
    
    
typedef struct AMAGE_DSP_IMG_TABLE_INDEXING_s_
{
         UINT8 YToneOffset;
         UINT8 YToneShift;
         UINT8 YToneBits;
         UINT8 UToneOffset;
         UINT8 UToneShift;
         UINT8 UToneBits;
         UINT8 VToneOffset;
         UINT8 VToneShift;
         UINT8 VToneBits;
         UINT8 pTable[4096];
} AMAGE_DSP_IMG_TABLE_INDEXING_s;

typedef struct sharpen_both_t_
{
         UINT8 Enable;
         UINT8 Mode;
         UINT16 EdgeThresh;
         UINT8 WideEdgeDetect;
         AMAGE_DSP_IMG_TABLE_INDEXING_s ThreeD;
         AMAGE_DSP_IMG_MAX_CHANGE_s MaxChange;
 } sharpen_both_t;
       
 typedef struct sharpen_noise_t_
 {
         UINT8 MaxChangeUp;
         UINT8 MaxChangeDown;
         FIR SpatialFir;
         sharpen_level_t LevelStrAdjust;
 } sharpen_noise_t;
    
 typedef struct SHARPEN_PKG_
 {
         UINT8 load_3d_flag;      
         FIR fir;
         CORING coring;
         sharpen_level_t ScaleCoring;
         sharpen_level_t MinCoringResult;
         sharpen_level_t CoringIndexScale;
         sharpen_both_t both;
         sharpen_noise_t noise;
  } SHARPEN_PKG;

 typedef struct AMAGE_DSP_IMG_FULL_ADAPTATION_s_
 {
         UINT8 AlphaMinUp;
         UINT8 AlphaMaxUp;
         UINT8 T0Up;
         UINT8 T1Up;
         UINT8 AlphaMinDown;
         UINT8 AlphaMaxDown;
         UINT8 T0Down;
         UINT8 T1Down;
         AMAGE_DSP_IMG_TABLE_INDEXING_s ThreeD;
} AMAGE_DSP_IMG_FULL_ADAPTATION_s;
    
typedef struct SA_ASF_INFO_
{
         UINT8 load_3d_flag;
         UINT8 Enable;
         FIR fir;
         UINT8 DirectionalDecideT0;
         UINT8 DirectionalDecideT1;
         AMAGE_DSP_IMG_FULL_ADAPTATION_s Adapt;
         sharpen_level_t LevelStrAdjust;
         sharpen_level_t T0T1Div;
         UINT8 MaxChangeUp;
         UINT8 MaxChangeDown;
         UINT8 Reserved;   /* to keep 32 alignment */  //not sure
         UINT16 Reserved1;  /* to keep 32 alignment */
} SA_ASF_INFO;

typedef struct SA_ASF_PKG_
{
         UINT8 select_mode;
         SHARPEN_PKG sa_info;
         SA_ASF_INFO asf_info;
} SA_ASF_PKG;

typedef struct AFS_EX_
{
         UINT8 af_horizontal_filter1_mode;
         UINT8 af_filter1_select;
         UINT8 af_horizontal_filter1_stage1_enb;
         UINT8 af_horizontal_filter1_stage2_enb;
         UINT8 af_horizontal_filter1_stage3_enb;
         INT16 af_horizontal_filter1_gain[7];
         UINT16 af_horizontal_filter1_shift[4];
         UINT16 af_horizontal_filter1_bias_off;
         UINT16 af_horizontal_filter1_thresh;
         UINT16 af_vertical_filter1_thresh;
         UINT8 af_horizontal_filter2_mode;
         UINT8 af_filter2_select;
         UINT8 af_horizontal_filter2_stage1_enb;
         UINT8 af_horizontal_filter2_stage2_enb;
         UINT8 af_horizontal_filter2_stage3_enb;
         INT16 af_horizontal_filter2_gain[7];
         UINT16 af_horizontal_filter2_shift[4];
         UINT16 af_horizontal_filter2_bias_off;
         UINT16 af_horizontal_filter2_thresh;
         UINT16 af_vertical_filter2_thresh;
         UINT16 af_tile_fv1_horizontal_shift;
         UINT16 af_tile_fv1_vertical_shift;
         UINT16 af_tile_fv1_horizontal_weight;
         UINT16 af_tile_fv1_vertical_weight;
         UINT16 af_tile_fv2_horizontal_shift;
         UINT16 af_tile_fv2_vertical_shift;
         UINT16 af_tile_fv2_horizontal_weight;
         UINT16 af_tile_fv2_vertical_weight;    
} AFS_EX;
    
typedef struct EC_INFO_
{
        //int agc_index;
        //int shutter_index;
        float GainFactor;
        float ShutterSpeed;
        int iris_index;
        UINT16 dgain;
} EC_INFO;
    
typedef struct CFA_TILE_INFO_
{
        UINT16 cfa_tile_num_col;
        UINT16 cfa_tile_num_row;
        UINT16 cfa_tile_col_start;
        UINT16 cfa_tile_row_start;
        UINT16 cfa_tile_width;
        UINT16 cfa_tile_height;
        UINT16 cfa_tile_active_width;
        UINT16 cfa_tile_active_height;
        UINT16 cfa_tile_cfa_y_shift;
        UINT16 cfa_tile_rgb_y_shift;
        UINT16 cfa_tile_min_max_shift;
} CFA_TILE_INFO;
    
typedef struct statistics_config_t_
{
        UINT16 AwbTileNumCol;
        UINT16 AwbTileNumRow;
        UINT16 AwbTileColStart;
        UINT16 AwbTileRowStart;
        UINT16 AwbTileWidth;
        UINT16 AwbTileHeight;
        UINT16 AwbTileActiveWidth;
        UINT16 AwbTileActiveHeight;
        UINT16 AwbPixMinValue;
        UINT16 AwbPixMaxValue;
        UINT16 AeTileNumCol;
        UINT16 AeTileNumRow;
        UINT16 AeTileColStart;
        UINT16 AeTileRowStart;
        UINT16 AeTileWidth;
        UINT16 AeTileHeight;
        UINT16 AePixMinValue;
        UINT16 AePixMaxValue;
        UINT16 AfTileNumCol;
        UINT16 AfTileNumRow;
        UINT16 AfTileColStart;
        UINT16 AfTileRowStart;
        UINT16 AfTileWidth;
        UINT16 AfTileHeight;
        UINT16 AfTileActiveWidth;
        UINT16 AfTileActiveHeight;
} statistics_config_t;
typedef struct TILE_PKG_
{
        statistics_config_t stat_config_info;

} TILE_PKG;
    
typedef struct hdr_mixer_config_t_
{
        UINT32 mixer_mode;
        UINT8 radius;
        UINT8 luma_weight_red;
        UINT8 luma_weight_green;
        UINT8 luma_weight_blue;
        UINT16 threshold;
        UINT8 thresh_delta;
        UINT8 long_exposure_shift;
} hdr_mixer_config_t; 

typedef struct hdr_mixer_setup_
{
        UINT8 auto_mode;
        hdr_mixer_config_t mixer_config[4];
} hdr_mixer_setup;

typedef struct HDR_BLC_INFO_
{
        UINT8 auto_mode;
        blc_level_t blc_offset[4];
} HDR_BLC_INFO;
 
typedef struct le_curve_config_t_
{
        UINT16 strength;
        UINT16 width;
} le_curve_config_t;

typedef struct HDR_CFA_INFO_
{
        UINT8 auto_mode;
        CFA cfa_noise_filter_setup[4];
} HDR_CFA_INFO;

typedef struct HDR_SHT_INFO_      
{
        UINT8 auto_mode;
        UINT8 index_or_row;
        int shutter_index[4];
        int shutter_row[4];
        UINT16 ae_target[4];       
} HDR_SHT_INFO;

typedef struct HDR_LE_INFO_
{
        UINT8 auto_mode;
        le_curve_config_t le_config[4];
        LE le_info[4];

} HDR_LE_INFO;

typedef struct HDR_TONE_INFO_
{
        UINT8 auto_mode;
        float alpha;
        UINT16 speed;
        Tone tone;
} HDR_TONE_INFO;

typedef struct out_lut_level_t_
{
         UINT8 org_t0;
         UINT8 org_t1;
         UINT16 tgt_t0;
         UINT16 tgt_t1;
} out_lut_level_t;

typedef struct out_lut_table_t_
{
         UINT16 table[256];

} out_lut_table_t;
    
 typedef struct HDR_ALPHA_PKG_
{
         UINT8 auto_mode;
         UINT8 select[4];
         out_lut_level_t outlut_level[4];
         out_lut_table_t outlut_table[4];
         
} HDR_ALPHA_PKG;
typedef struct cc_
{
        //UINT8 cc_reg[18752];    //different from cloudy, cc is for HDR
        UINT8 cc_matirx[17536];  //for 3d
} cc;
typedef struct AmageCC_
{
        UINT8 cc_reg[18752];
        UINT8 cc_3d[17536];
} AmageCC;
typedef struct HDR_CT_PKG_
{
         UINT8 cc_load_flag;
         UINT8 tone_load_flag;
         Tone tone[4];
         cc cc_maxtirx[4];
} HDR_CT_PKG;
    
typedef struct HDR_WB_PKG_
{
         //AMAGE_DSP_IMG_WB_GAIN_s wb[4];  //marked by htchiu on 2013/10/31
        WB wb[4];
} HDR_WB_PKG;

typedef struct HDR_AGC_INFO_
{
         int agc[4];
         UINT16 dgain[4];
} HDR_AGC_INFO;

typedef struct rgb_histogram_stat_t_
{
        UINT32 his_bin_y[64];
        UINT32 his_bin_r[64];
        UINT32 his_bin_g[64];
        UINT32 his_bin_b[64];
} rgb_histogram_stat_t;

typedef struct cfa_histogram_stat_t_
{
        UINT32 his_bin_r[64];
        UINT32 his_bin_g[64];
        UINT32 his_bin_b[64];
        UINT32 his_bin_y[64];
} cfa_histogram_stat_t;

typedef struct histogram_stat_t_
{
        cfa_histogram_stat_t cfa_histogram;
        rgb_histogram_stat_t rgb_histogram;
} histogram_stat_t;

typedef struct HDR_HISTO_PKG_
{
        histogram_stat_t dsp_histo_data[4];
} HDR_HISTO_PKG;

typedef struct AAA_INFO_PKG_
{
        UINT32 ae_lin_y[96];
        UINT32 awb_r[1024];
        UINT32 awb_g[1024];
        UINT32 awb_b[1024];
        UINT16 af_fv2[256];
} AAA_INFO_PKG;

typedef struct FPN_DETECT_INFO_
{
        UINT32 cap_width;
        UINT32 cap_height;
        UINT32 badpix_type; // hot pixel or cold pixel
        UINT32 block_h;
        UINT32 block_w;
        UINT32 upper_thres;
        UINT32 lower_thres;
        UINT32 detect_times;
        UINT32 shutter_idx;
        UINT32 agc_idx;
} FPN_DETECT_INFO;

typedef struct FPN_CORRECT_INFO_
{
        UINT32 width;
        UINT32 height;
} FPN_CORRECT_INFO;

typedef struct FPN_INFO_
{
        UINT8 mode;
        FPN_CORRECT_INFO fpn_correct_info;
        FPN_DETECT_INFO fpn_detect_info;
} FPN_INFO;

typedef struct FPN_PKG_
{
        FPN_INFO fpn_info;
        UINT8 badpixmap_buf[];

} FPN_PKG;

typedef struct LENS_CALI_PKG_
{
        UINT8 gain_shift;
        UINT16 vignette_red_gain_addr[33*33];
        UINT16 vignette_green_even_gain_addr[33*33];
        UINT16 vignette_green_odd_gain_addr[33*33];
        UINT16 vignette_blue_gain_addr[33*33];
} LENS_CALI_PKG;

typedef struct _ITUNER_SYSTEM_t_ {
    char    AmbacamRev[16];
    char    ChipRev[8];
    char    SensorId[32];
    char    UserMode[32];
    char    TuningMode[32];
    char    TuningModeExt[32];
    //char    RawPath[64];
    char    ExtConfigPath[64];    
    UINT16  RawWidth;
    UINT16  RawHeight;
    UINT16  RawPitch;
    UINT16  RawResolution;
    UINT16  RawBayer;
    UINT16  MainWidth;
    UINT16  MainHeight;
    UINT16  InputPicCnt;
    UINT16  CompressedRaw;
    UINT16  SensorReadoutMode;
    UINT32  RawStartX;
    UINT32  RawStartY;
    UINT8   HSubSampleFactorNum;
    UINT8   HSubSampleFactorDen;
    UINT8   VSubSampleFactorNum;
    UINT8   VSubSampleFactorDen;
    UINT8   EnableRaw2Raw;
    UINT8   JpgQuality;
} ITUNER_SYSTEM_t;
 typedef struct _ITUNER_WARP_t_ {
    UINT8   Enable;
    UINT32  CalibVersion;
    int     HorizontalGridNumber;
    int     VerticalGridNumber;
    int     TileWidthExponent;
    int     TileHeightExponent;
    UINT32  VinSensorStartX;
    UINT32  VinSensorStartY;
    UINT32  VinSensorWidth;
    UINT32  VinSensorHeight;
    UINT8   VinSensorHSubSampleFactorNum;
    UINT8   VinSensorHSubSampleFactorDen;
    UINT8   VinSensorVSubSampleFactorNum;
    UINT8   VinSensorVSubSampleFactorDen;
    //char    WarpGridTablePath[64];
    UINT8 WarpGridTable[4*96*64];
    int table_size;
} ITUNER_WARP_t;
 typedef struct _ITUNER_DZOOM_t_ {
    UINT8  Enable;
    UINT32 ActualLeftTopX;
    UINT32 ActualLeftTopY;
    UINT32 ActualRightBotX;
    UINT32 ActualRightBotY;
    int    HorSkewPhaseInc;
    UINT16 DummyWindowLeftX;
    UINT16 DummyWindowTopY;
    UINT16 DummyWindowWidth;
    UINT16 DummyWindowHeight;
    UINT16 CfaOutputWidth;
    UINT16 CfaOutputHeight;
    UINT16 R2rOutputWidth;
    UINT16 R2rOutputHeigh;
} ITUNER_DZOOM_t;
 typedef struct _ITUNER_VIGNETTE_t_ {
    UINT8    Enable;
    UINT8    GainShift;
    UINT8    StrengthEffectMode;
    UINT32   ChromaRatio;
    UINT32   Strength;
    UINT32   CalibVersion;
    int      CalibTableWidth;
    int      CalibTableHeight;
    UINT32   CalibVinStartX;
    UINT32   CalibVinStartY;
    UINT32   CalibVinWidth;
    UINT32   CalibVinHeight;
    UINT8    CailbVinHSubSampleFactorNum;
    UINT8    CailbVinHSubSampleFactorDen;
    UINT8    CailbVinVSubSampleFactorNum;
    UINT8    CailbVinVSubSampleFactorDen;
    //char     CalibTablePath[64];
    UINT8 CalibTable[(2*65*49)*4];
    int table_size;
} ITUNER_VIGNETTE_t;
 typedef struct _ITUNER_CHROMA_ABERRATION_t_ {
    UINT8   Enable;
    UINT32  CalibVersion;
    int     HorizontalGridNumber;
    int     VerticalGridNumber;
    int     TileWidthExponent;
    int     TileHeightExponent;
    UINT32  VinSensorStartX;
    UINT32  VinSensorStartY;
    UINT32  VinSensorWidth;
    UINT32  VinSensorHeight;
    UINT8   VinSensorHSubSampleFactorNum;
    UINT8   VinSensorHSubSampleFactorDen;
    UINT8   VinSensorVSubSampleFactorNum;
    UINT8   VinSensorVSubSampleFactorDen;
    INT16  RedScaleFactor;
    INT16  BlueScaleFactor;
    //char    CaGridTablePath[64];
    UINT8 CaGridTable[4*96*64];
    int table_size;
} ITUNER_CHROMA_ABERRATION_t;
 typedef struct _ITUNER_FPN_t_ {
    UINT8  Enable;
    UINT32 CalibVersion;
    UINT32 CailbVinStartX;
    UINT32 CailbVinStartY;
    UINT32 CailbVinWidth;
    UINT32 CailbVinHeight;
    UINT8  CailbVinHSubSampleFactorNum;
    UINT8  CailbVinHSubSampleFactorDen;
    UINT8  CailbVinVSubSampleFactorNum;
    UINT8  CailbVinVSubSampleFactorDen;
    //char   MapPath[64];
    UINT8 fpnTable[4000*3000];
    int table_size;
} ITUNER_FPN_t;
 
 typedef struct _ITUNER_FPN_param_ {
    UINT8  Enable;
    UINT32 CalibVersion;
    UINT32 CailbVinStartX;
    UINT32 CailbVinStartY;
    UINT32 CailbVinWidth;
    UINT32 CailbVinHeight;
    UINT8  CailbVinHSubSampleFactorNum;
    UINT8  CailbVinHSubSampleFactorDen;
    UINT8  CailbVinVSubSampleFactorNum;
    UINT8  CailbVinVSubSampleFactorDen;
} ITUNER_FPN_param;

typedef struct _TUNING_TABLE_s
{
    UINT8 SharpenBothThreeDTable[4096];
    UINT8 FinalSharpenBothThreeDTable[4096];
    UINT8 AsfInfoThreeDTable[4096];
    UINT8 HisoAsfThreeDTable[4096];
    UINT8 HisoHighAsfThreeDTable[4096];
    UINT8 HisoLowAsfThreeDTable[4096];
    UINT8 HisoMed1AsfThreeDTable[4096];
    UINT8 HisoMed2AsfThreeDTable[4096];
    UINT8 HisoLi2ndAsfThreeDTable[4096];
    UINT8 HisoChromaAsfThreeDTable[4096];
    UINT8 HisoHighSharpenBothThreeDTable[4096];
    UINT8 HisoMedSharpenBothThreeDTable[4096];
    UINT8 HisoLiso1SharpenBothThreeDTable[4096];
    UINT8 HisoLiso2SharpenBothThreeDTable[4096];
    UINT8 HisoChromaFilterMedCombineThreeDTable[4096];
    UINT8 HisoChromaFilterLowCombineThreeDTable[4096];
    UINT8 HisoChromaFilterVeryLowCombineThreeDTable[4096];
    UINT8 HisoLumaNoiseCombineThreeDTable[4096];
    UINT8 HisoLowASFCombineThreeDTable[4096];
    UINT8 HighIsoCombineThreeDTable[4096];
}TUNING_TABLE;
 
#endif /* _AMAGE_typedef_struct_H */
