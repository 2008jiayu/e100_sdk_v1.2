#ifndef __H_APPLIB_NET_FIFO__
#define __H_APPLIB_NET_FIFO__

/**
 * @defgroup ApplibNet_Fifo
 * @brief Interfaces for net fifo service
 *
 *
 */

/**
 * @addtogroup ApplibNet_Fifo
 * @ingroup NetService
 * @{
 */

#include <net/NetFifo.h>

#define APPLIB_NETFIFO_MAX_FN_SIZE (48)

/**
 * Net stream mode
 */
typedef enum _NET_STREAM_MODE_e_{
    NET_STREAM_MODE_AV,         /**< net stream with both video and audio */
    NET_STREAM_MODE_VIDEO_ONLY, /**< net stream with video only */
    NET_STREAM_MODE_MAX         /**< net stream mode boundary */
} NET_STREAM_MODE_e;
/**
 * Network Playback op type
 */
typedef enum _APPLIB_NETFIFO_PLAYBACK_OP_e_ {
    APPLIB_NETFIFO_PLAYBACK_OPEN = 1,      /**< open file for playback. */
    APPLIB_NETFIFO_PLAYBACK_PLAY,          /**< start playback. */
    APPLIB_NETFIFO_PLAYBACK_STOP,          /**< stop playback */
    APPLIB_NETFIFO_PLAYBACK_RESET,         /**< reset playback */
    APPLIB_NETFIFO_PLAYBACK_PAUSE,         /**< pause playback */
    APPLIB_NETFIFO_PLAYBACK_RESUME,        /**< resume playback */
    APPLIB_NETFIFO_PLAYBACK_CONFIG,        /**< enabling playback stream */
} APPLIB_NETFIFO_PLAYBACK_OP_e;


/**
 * Applib Net Playback Message Structure Definitions
 */
typedef struct _APP_NET_PB_MESSAGE_s_ {
    UINT32 MessageID;       /**< Message Id.*/
    UINT32 MessageData[2];  /**< Message data.*/
    //char Filename[MAX_FILENAME_LENGTH];
    char Filename[APPLIB_NETFIFO_MAX_FN_SIZE];
} APP_NET_PB_MESSAGE_s;

typedef void (*APPLIB_NETFIFO_PB_f)(void);

/*
 *  Check the statistic of RR and change bitrate accordingly
 */
typedef struct _NET_STREAM_REPORT_s_ {
    unsigned int FrameLost;
    unsigned int Jitter;
    double PGDelay;
} NET_STREAM_REPORT_s;

typedef struct _NET_STREAM_BITRATE_STAT_s_ {
    unsigned int CurBitRate;    //indicate the current bitrate
    unsigned int MaxStableBR;   //max stable bitrate
    unsigned int LastBitRate;   //indicate the bitrate when last RR come
    unsigned int MaxBitRate;    //the max bitrate we can reach in this sensor_config
    unsigned int NetBandwidth;  //network bandwidth
    unsigned int ZeroLost;      //indicate how many consecutive fr_lost=0 we got
    unsigned int LastFrameLost;    //fr_lost of last RR
    UINT8 InMiddleOfIncrement;
    UINT8 InMiddleOfDecrement;
    UINT8 Inited;              //indicate that whether some value is inited or not
} NET_STREAM_BITRATE_STAT_s;

/**
 *  Structure Definitions of SPS and PPS information
 */
typedef struct _APPLIB_NETFIFO_SPS_PPS_s_ {
    UINT8 Sps[64];
    UINT32 SpsLen;
    UINT8 Pps[32];
    UINT32 PpsLen;
} APPLIB_NETFIFO_SPS_PPS_s;


/**
 *  @brief Initialize net fifo module
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetFifo_Init(void);

/**
 *  @brief Start RTSP server
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetFifo_StartRTSPServer(void);

/**
 *  @brief Stop RTSP server
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetFifo_StopRTSPServer(void);


/**
 *  @brief Notify netFifo module that the state of application has been changed
 *
 *  @param [in] state the state that application has changed to
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetFifo_NotifyAppStateChange(AMP_NETFIFO_NOTIFY_TYPE_e state);

/**
 *  @brief Set net stream mode
 *
 *  @param [in] mode the net stream mode
 *
 *  @return 0 success, <0 failure
 */
int AppLibNetFifo_SetStreamMode(NET_STREAM_MODE_e mode);

/**
 *  @brief Get net stream mode
 *
 *  @return net stream mode
 */
NET_STREAM_MODE_e AppLibNetFifo_GetStreamMode(void);

int AppLibNetFifo_EnablePlayback(void);
int AppLibNetFifo_PlaybackSendMsg(APP_NET_PB_MESSAGE_s *msg, UINT32 waitOption);
int AppLibNetFifo_PlaybackRecvMsg(APP_NET_PB_MESSAGE_s *msg, UINT32 waitOption);
int AppLibNetFifo_PlaybackRegisterApp(APPLIB_NETFIFO_PB_f NetFifoPlayback);


#endif //__H_APPLIB_NET_FIFO__

