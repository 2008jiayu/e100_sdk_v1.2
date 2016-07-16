/**************************************************************************/ 
/*                                                                        */ 
/*            Copyright (c) 1996-2012 by Express Logic Inc.               */ 
/*                                                                        */ 
/*  This software is copyrighted by and is the sole property of Express   */ 
/*  Logic, Inc.  All rights, title, ownership, or other interests         */ 
/*  in the software remain the property of Express Logic, Inc.  This      */ 
/*  software may only be used in accordance with the corresponding        */ 
/*  license agreement.  Any unauthorized use, duplication, transmission,  */ 
/*  distribution, or disclosure of this software is expressly forbidden.  */ 
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */ 
/*  written consent of Express Logic, Inc.                                */ 
/*                                                                        */ 
/*  Express Logic, Inc. reserves the right to modify this software        */ 
/*  without notice.                                                       */ 
/*                                                                        */ 
/*  Express Logic, Inc.                     info@expresslogic.com         */
/*  11423 West Bernardo Court               http://www.expresslogic.com   */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   Device Storage Class                                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    ux_device_class_storage.h                           PORTABLE C      */ 
/*                                                           5.6          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron, Express Logic Inc.                                   */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file contains all the header and extern functions used by the  */
/*    USBX device storage class.                                          */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  07-01-2007     TCRG                     Initial Version 5.0           */ 
/*  11-11-2008     TCRG                     Modified comment(s), and      */ 
/*                                            added new read capacity     */ 
/*                                            response constants,         */ 
/*                                            resulting in version 5.2    */ 
/*  07-10-2009     TCRG                     Modified comment(s), and      */ 
/*                                            added trace logic,          */ 
/*                                            resulting in version 5.3    */ 
/*  06-13-2010     TCRG                     Modified comment(s),          */ 
/*                                            resulting in version 5.4    */ 
/*  09-01-2011     TCRG                     Modified comment(s),          */ 
/*                                            resulting in version 5.5    */ 
/*  10-10-2012     TCRG                     Modified comment(s),          */ 
/*                                            resulting in version 5.6    */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef UX_DEVICE_CLASS_STORAGE_H
#define UX_DEVICE_CLASS_STORAGE_H

/* Define User configurable Storage Class constants.  */

#ifndef UX_MAX_SLAVE_LUN
#define UX_MAX_SLAVE_LUN                                            2
#endif

/* Define Storage Class USB Class constants.  */

#define UX_SLAVE_CLASS_STORAGE_CLASS                                8
#define UX_SLAVE_CLASS_STORAGE_SUBCLASS_RBC                         1
#define UX_SLAVE_CLASS_STORAGE_SUBCLASS_SFF8020                     2
#define UX_SLAVE_CLASS_STORAGE_SUBCLASS_UFI                         4
#define UX_SLAVE_CLASS_STORAGE_SUBCLASS_SFF8070                     5
#define UX_SLAVE_CLASS_STORAGE_SUBCLASS_SCSI                        6
#define UX_SLAVE_CLASS_STORAGE_PROTOCOL_CBI                         0
#define UX_SLAVE_CLASS_STORAGE_PROTOCOL_CB                          1
#define UX_SLAVE_CLASS_STORAGE_PROTOCOL_BO                          0x50

/* Define Storage Class USB MEDIA types.  */
#define UX_SLAVE_CLASS_STORAGE_MEDIA_FAT_DISK                       0
#define UX_SLAVE_CLASS_STORAGE_MEDIA_CDROM                          5
#define UX_SLAVE_CLASS_STORAGE_MEDIA_OPTICAL_DISK                   7
#define UX_SLAVE_CLASS_STORAGE_MEDIA_IOMEGA_CLICK                   0x55



/* Define Storage Class SCSI command constants.  */

#define UX_SLAVE_CLASS_STORAGE_SCSI_TEST_READY                      0x00
#define UX_SLAVE_CLASS_STORAGE_SCSI_REQUEST_SENSE                   0x03
#define UX_SLAVE_CLASS_STORAGE_SCSI_FORMAT                          0x04
#define UX_SLAVE_CLASS_STORAGE_SCSI_INQUIRY                         0x12
#define UX_SLAVE_CLASS_STORAGE_SCSI_MODE_SENSE_SHORT                0x1a
#define UX_SLAVE_CLASS_STORAGE_SCSI_START_STOP                      0x1b
#define UX_SLAVE_CLASS_STORAGE_SCSI_PREVENT_ALLOW_MEDIA_REMOVAL     0x1e
#define UX_SLAVE_CLASS_STORAGE_SCSI_READ_FORMAT_CAPACITY            0x23
#define UX_SLAVE_CLASS_STORAGE_SCSI_READ_CAPACITY                   0x25
#define UX_SLAVE_CLASS_STORAGE_SCSI_READ16                          0x28
#define UX_SLAVE_CLASS_STORAGE_SCSI_WRITE16                         0x2a
#define UX_SLAVE_CLASS_STORAGE_SCSI_VERIFY                          0x2f
#define UX_SLAVE_CLASS_STORAGE_SCSI_READ_TOC                        0x43
#define UX_SLAVE_CLASS_STORAGE_SCSI_GET_CONFIGURATION               0x46
#define UX_SLAVE_CLASS_STORAGE_SCSI_MODE_SELECT                     0x55
#define UX_SLAVE_CLASS_STORAGE_SCSI_MODE_SENSE                      0x5a
#define UX_SLAVE_CLASS_STORAGE_SCSI_READ32                          0xa8 
#define UX_SLAVE_CLASS_STORAGE_SCSI_WRITE32                         0xaa
#define UX_SLAVE_CLASS_STORAGE_SCSI_READ_CD                         0xbe



/* Define Storage Class SCSI command block wrapper constants.  */

#define UX_SLAVE_CLASS_STORAGE_CBW_SIGNATURE_MASK                   0x43425355
#define UX_SLAVE_CLASS_STORAGE_CBW_SIGNATURE                        0
#define UX_SLAVE_CLASS_STORAGE_CBW_TAG                              4
#define UX_SLAVE_CLASS_STORAGE_CBW_DATA_LENGTH                      8
#define UX_SLAVE_CLASS_STORAGE_CBW_FLAGS                            12
#define UX_SLAVE_CLASS_STORAGE_CBW_LUN                              13
#define UX_SLAVE_CLASS_STORAGE_CBW_CB_LENGTH                        14
#define UX_SLAVE_CLASS_STORAGE_CBW_CB                               15
#define UX_SLAVE_CLASS_STORAGE_CBW_LENGTH                           31


/* Define Storage Class SCSI response status wrapper constants.  */

#define UX_SLAVE_CLASS_STORAGE_CSW_SIGNATURE_MASK                   0x53425355
#define UX_SLAVE_CLASS_STORAGE_CSW_SIGNATURE                        0
#define UX_SLAVE_CLASS_STORAGE_CSW_TAG                              4
#define UX_SLAVE_CLASS_STORAGE_CSW_DATA_RESIDUE                     8
#define UX_SLAVE_CLASS_STORAGE_CSW_STATUS                           12
#define UX_SLAVE_CLASS_STORAGE_CSW_LENGTH                           13


/* Define Storage Class SCSI get configuration command constants.  */ 

#define UX_SLAVE_CLASS_STORAGE_GET_CONFIGURATION_OPERATION                    0
#define UX_SLAVE_CLASS_STORAGE_GET_CONFIGURATION_RT                            1
#define UX_SLAVE_CLASS_STORAGE_GET_CONFIGURATION_START_FEATURE_NUM           2
#define UX_SLAVE_CLASS_STORAGE_GET_CONFIGURATION_ALLOCATION_LENGTH           7


/* Define Storage Class SCSI Read CD command constants.  */ 

#define UX_SLAVE_CLASS_STORAGE_READ_CD_OPERATION                        0
#define UX_SLAVE_CLASS_STORAGE_READ_CD_START_LBA                        2
#define UX_SLAVE_CLASS_STORAGE_READ_CD_BLOCK_CNT                        6
#define UX_SLAVE_CLASS_STORAGE_READ_CD_SYNC_HEADER_USR_ECCC2           9
#define UX_SLAVE_CLASS_STORAGE_READ_CD_SUB_CHANNEL                      10
#define UX_SLAVE_CLASS_STORAGE_READ_CD_CTRL_BYTE                        11

/* Define Storage Class SCSI Read TOC command constants.  */ 

#define UX_SLAVE_CLASS_STORAGE_READ_TOC_OPERATION                        0
#define UX_SLAVE_CLASS_STORAGE_READ_TOC_MSF                              1
#define UX_SLAVE_CLASS_STORAGE_READ_TOC_FORMAT                          2
#define UX_SLAVE_CLASS_STORAGE_READ_TOC_TRACK_SESSION_NUM               6
#define UX_SLAVE_CLASS_STORAGE_READ_TOC_ALLOCATE_LEN                    7
#define UX_SLAVE_CLASS_STORAGE_READ_TOC_CTRL_BYTE                        9

/* Define Storage Class SCSI inquiry command constants.  */ 

#define UX_SLAVE_CLASS_STORAGE_INQUIRY_OPERATION                    0
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_LUN                          1
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_PAGE_CODE                    2
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_ALLOCATION_LENGTH            4
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_COMMAND_LENGTH_UFI           12
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_COMMAND_LENGTH_SBC           06


/* Define Storage Class SCSI inquiry response constants.  */

#define UX_SLAVE_CLASS_STORAGE_INQUIRY_RESPONSE_PERIPHERAL_TYPE     0
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_RESPONSE_REMOVABLE_MEDIA     1
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_RESPONSE_DATA_FORMAT         3
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_RESPONSE_ADDITIONAL_LENGTH   4
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_RESPONSE_VENDOR_INFORMATION  8
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_RESPONSE_PRODUCT_ID          16
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_RESPONSE_PRODUCT_REVISION    32
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_RESPONSE_LENGTH              96
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_RESPONSE_LENGTH_CD_ROM       0x5b


/* Define Storage Class SCSI start/stop command constants.  */

#define UX_SLAVE_CLASS_STORAGE_START_STOP_OPERATION                 0
#define UX_SLAVE_CLASS_STORAGE_START_STOP_LBUFLAGS                  1
#define UX_SLAVE_CLASS_STORAGE_START_STOP_START_BIT                 4
#define UX_SLAVE_CLASS_STORAGE_START_STOP_COMMAND_LENGTH_UFI        12
#define UX_SLAVE_CLASS_STORAGE_START_STOP_COMMAND_LENGTH_SBC        6


/* Define Storage Class SCSI mode sense command constants.  */

#define UX_SLAVE_CLASS_STORAGE_MODE_SENSE_OPERATION                 0
#define UX_SLAVE_CLASS_STORAGE_MODE_SENSE_LUN                       1
#define UX_SLAVE_CLASS_STORAGE_MODE_SENSE_DBD                       1
#define UX_SLAVE_CLASS_STORAGE_MODE_SENSE_PC_PAGE_CODE              2
#define UX_SLAVE_CLASS_STORAGE_MODE_SENSE_PARAMETER_LIST_LENGTH     7
#define UX_SLAVE_CLASS_STORAGE_MODE_SENSE_COMMAND_LENGTH_UFI        12
#define UX_SLAVE_CLASS_STORAGE_MODE_SENSE_COMMAND_LENGTH_SBC        12


/* Define Storage Class SCSI request sense command constants.  */

#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_OPERATION              0
#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_LUN                    1
#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_ALLOCATION_LENGTH      4
#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_COMMAND_LENGTH_UFI     12
#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_COMMAND_LENGTH_SBC     12


/* Define Storage Class request sense response constants.  */

#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_RESPONSE_ERROR_CODE     0
#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_RESPONSE_SENSE_KEY      2
#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_RESPONSE_INFORMATION    3
#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_RESPONSE_ADD_LENGTH     7
#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_RESPONSE_CODE           12
#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_RESPONSE_CODE_QUALIFIER 13
#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_RESPONSE_LENGTH         18


/* Define Storage Class read capacity command constants.  */

#define UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_OPERATION              0
#define UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_LUN                    1
#define UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_LBA                    2
#define UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_COMMAND_LENGTH_UFI     12
#define UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_COMMAND_LENGTH_SBC     10


/* Define Storage Class read capacity response constants.  */

#define UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_RESPONSE_LAST_LBA      0
#define UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_RESPONSE_BLOCK_SIZE    4
#define UX_SLAVE_CLASS_STORAGE_READ_CAPACITY_RESPONSE_LENGTH        8

/* Define Storage Class read capacity response constants.  */

#define UX_SLAVE_CLASS_STORAGE_READ_FORMAT_CAPACITY_RESPONSE_SIZE           0
#define UX_SLAVE_CLASS_STORAGE_READ_FORMAT_CAPACITY_RESPONSE_LAST_LBA       4
#define UX_SLAVE_CLASS_STORAGE_READ_FORMAT_CAPACITY_RESPONSE_DESC_CODE      8
#define UX_SLAVE_CLASS_STORAGE_READ_FORMAT_CAPACITY_RESPONSE_BLOCK_SIZE     8
#define UX_SLAVE_CLASS_STORAGE_READ_FORMAT_CAPACITY_RESPONSE_LENGTH         12

/* Define Storage Class test unit read command constants.  */

#define UX_SLAVE_CLASS_STORAGE_TEST_READY_OPERATION                 0
#define UX_SLAVE_CLASS_STORAGE_TEST_READY_LUN                       1
#define UX_SLAVE_CLASS_STORAGE_TEST_READY_COMMAND_LENGTH_UFI        12
#define UX_SLAVE_CLASS_STORAGE_TEST_READY_COMMAND_LENGTH_SBC        6


/* Define Storage Class SCSI read command constants.  */

#define UX_SLAVE_CLASS_STORAGE_READ_OPERATION                       0
#define UX_SLAVE_CLASS_STORAGE_READ_LUN                             1
#define UX_SLAVE_CLASS_STORAGE_READ_LBA                             2
#define UX_SLAVE_CLASS_STORAGE_READ_TRANSFER_LENGTH_32              6
#define UX_SLAVE_CLASS_STORAGE_READ_TRANSFER_LENGTH_16              7
#define UX_SLAVE_CLASS_STORAGE_READ_COMMAND_LENGTH_UFI              12
#define UX_SLAVE_CLASS_STORAGE_READ_COMMAND_LENGTH_SBC              10


/* Define Storage Class SCSI write command constants.  */

#define UX_SLAVE_CLASS_STORAGE_WRITE_OPERATION                      0
#define UX_SLAVE_CLASS_STORAGE_WRITE_LUN                            1
#define UX_SLAVE_CLASS_STORAGE_WRITE_LBA                            2
#define UX_SLAVE_CLASS_STORAGE_WRITE_TRANSFER_LENGTH_32             6
#define UX_SLAVE_CLASS_STORAGE_WRITE_TRANSFER_LENGTH_16             7
#define UX_SLAVE_CLASS_STORAGE_WRITE_COMMAND_LENGTH_UFI             12
#define UX_SLAVE_CLASS_STORAGE_WRITE_COMMAND_LENGTH_SBC             10


/* Define Storage Class SCSI sense key definition constants.  */

#define UX_SLAVE_CLASS_STORAGE_SENSE_KEY_NO_SENSE                   0x0
#define UX_SLAVE_CLASS_STORAGE_SENSE_KEY_RECOVERED_ERROR            0x1
#define UX_SLAVE_CLASS_STORAGE_SENSE_KEY_NOT_READY                  0x2
#define UX_SLAVE_CLASS_STORAGE_SENSE_KEY_MEDIUM_ERROR               0x3
#define UX_SLAVE_CLASS_STORAGE_SENSE_KEY_HARDWARE_ERROR             0x4
#define UX_SLAVE_CLASS_STORAGE_SENSE_KEY_ILLEGAL_REQUEST            0x5
#define UX_SLAVE_CLASS_STORAGE_SENSE_KEY_UNIT_ATTENTION             0x6
#define UX_SLAVE_CLASS_STORAGE_SENSE_KEY_DATA_PROTECT               0x7
#define UX_SLAVE_CLASS_STORAGE_SENSE_KEY_BLANK_CHECK                0x8
#define UX_SLAVE_CLASS_STORAGE_SENSE_KEY_ABORTED_COMMAND            0x0b
#define UX_SLAVE_CLASS_STORAGE_SENSE_KEY_VOLUME_OVERFLOW            0x0d
#define UX_SLAVE_CLASS_STORAGE_SENSE_KEY_MISCOMPARE                 0x0e

/* Define Storage Class SCSI ASC return codes.  */
#define UX_SLAVE_CLASS_STORAGE_ASC_NO_INFO                      0x00
#define UX_SLAVE_CLASS_STORAGE_ASC_WRITE_ERROR                  0x03
#define UX_SLAVE_CLASS_STORAGE_ASC_LU_NOT_READY                 0x04
#define UX_SLAVE_CLASS_STORAGE_ASC_LU_COMMUNICATION_FAILED      0x08
#define UX_SLAVE_CLASS_STORAGE_ASC_READ_ERROR                   0x11
#define UX_SLAVE_CLASS_STORAGE_ASC_MISCOMPARE_DURING_VERIFY     0x1D
#define UX_SLAVE_CLASS_STORAGE_ASC_INVALID_COMMAND              0x20
#define UX_SLAVE_CLASS_STORAGE_ASC_INVALID_FIELD_IN_CDB         0x24
#define UX_SLAVE_CLASS_STORAGE_ASC_INVALID_PARAMETER            0x26
#define UX_SLAVE_CLASS_STORAGE_ASC_WRITE_PROTECTED              0x27
#define UX_SLAVE_CLASS_STORAGE_ASC_NOT_READY_TO_READY           0x28
#define UX_SLAVE_CLASS_STORAGE_ASC_MODE_PARAM_CHANGED           0x2A
#define UX_SLAVE_CLASS_STORAGE_ASC_SAVING_PARAM_NOT_SUPPORT     0x39
#define UX_SLAVE_CLASS_STORAGE_ASC_MEDIUM_NOT_PRESENT           0x3A
#define UX_SLAVE_CLASS_STORAGE_ASC_LOAD_EJECT_ERROR             0x53

/* Define Storage Class SCSI ASCQ return codes.  */
#define UX_SLAVE_CLASS_STORAGE_ASCQ_NO_INFO                     0x00
#define UX_SLAVE_CLASS_STORAGE_ASCQ_MISCOMPARE_DURING_VERIFY    0x00
#define UX_SLAVE_CLASS_STORAGE_ASCQ_INVALID_CDB                 0x00
#define UX_SLAVE_CLASS_STORAGE_ASCQ_INVALID_PARAMETER           0x02
#define UX_SLAVE_CLASS_STORAGE_ASCQ_INVALID_COMMAND             0x00
#define UX_SLAVE_CLASS_STORAGE_ASCQ_LU_NOT_READY                0x02
#define UX_SLAVE_CLASS_STORAGE_ASCQ_WRITE_ERROR                 0x00
#define UX_SLAVE_CLASS_STORAGE_ASCQ_READ_ERROR                  0x00
#define UX_SLAVE_CLASS_STORAGE_ASCQ_LOAD_EJECT_ERROR            0x00
#define UX_SLAVE_CLASS_STORAGE_ASCQ_NOT_READY_TO_READY          0x00
#define UX_SLAVE_CLASS_STORAGE_ASCQ_MEDIUM_NOT_PRESENT          0x00
#define UX_SLAVE_CLASS_STORAGE_ASCQ_SAVING_PARAM_NOT_SUPPORT    0x00
#define UX_SLAVE_CLASS_STORAGE_ASCQ_MODE_PARAM_CHANGED          0x01
#define UX_SLAVE_CLASS_STORAGE_ASCQ_WRITE_PROTECTED             0x00
#define UX_SLAVE_CLASS_STORAGE_ASCQ_INIT_REQUIRED               0x02
#define UX_SLAVE_CLASS_STORAGE_ASCQ_INVLD_CMD_OPRT_CODE         0x00
#define UX_SLAVE_CLASS_STORAGE_ASCQ_OPRT_IN_PRGS                0x07

/* Define Storage Class CSW status.  */

#define UX_SLAVE_CLASS_STORAGE_CSW_PASSED                           0
#define UX_SLAVE_CLASS_STORAGE_CSW_FAILED                           1
#define UX_SLAVE_CLASS_STORAGE_CSW_PHASE_ERROR                      2


/* Define generic SCSI values.  */

#define UX_SLAVE_CLASS_STORAGE_REQUEST_SENSE_RESPONSE_ERROR_CODE_VALUE  0x70
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_PAGE_CODE_STANDARD               0x00
#define UX_SLAVE_CLASS_STORAGE_INQUIRY_PERIPHERAL_TYPE                  0x00
#define UX_SLAVE_CLASS_STORAGE_RESET                                    0xff
#define UX_SLAVE_CLASS_STORAGE_GET_MAX_LUN                              0xfe
#define UX_SLAVE_CLASS_STORAGE_MAX_LUN                                  0
#define UX_SLAVE_CLASS_STORAGE_RESPONSE_LENGTH                          64


/* Define buffer length for IN/OUT pipes.  This should match the size of the endpoint maximum buffer size. */

#define UX_SLAVE_CLASS_STORAGE_BUFFER_SIZE                              UX_SLAVE_REQUEST_BULKIN_DATA_MAX_LENGTH
#define UX_SLAVE_CLASS_STORAGE_WRITE_BUFFER_SIZE                        UX_SLAVE_REQUEST_DATA_MAX_LENGTH
#define UX_SLAVE_CLASS_STORAGE_READ_CACHE_IN_NEXT_BUF                   0xCC
#define UX_SLAVE_CLASS_STORAGE_READ_CACHE_IN_PREV_BUF                   0xDD

// Define media_status
#define UX_SLAVE_CLASS_STORAGE_MEDIA_DISABL_ACCESS 0x0
#define UX_SLAVE_CLASS_STORAGE_MEDIA_ENABL_ACCESS  0x1
#define UX_SLAVE_CLASS_STORAGE_MEDIA_UNLOAD        0x2
#define UX_SLAVE_CLASS_STORAGE_MEDIA_LOAD          0x3

/*-----------------------------------
    for struct
-----------------------------------*/

#define BLK_ADDR_LEN            4
#define BLK_CNT                 2
#define PAR_LIST_LENGTH         2
#define ALLOC_LENGTH            2
#define PAGE_CODE_LIST          10
#define ASCII_INFO              10
#define SERI_NO                 10
#define OPE_MODE                10
#define MODE_PARA_LEN           2
#define BLK_DSCRP_LEN           2
#define TIME_LMT                2
#define BUS_INACT_LIM_LEN       2
#define DISCNCT_TM_LIM          2
#define CNCT_TM_LIM             2
#define MAX_BRST_SIZE           2
#define TRCK_NUM                2
#define CHG_SEC_NUM             2
#define CHG_TRCK_NUM            2
#define CHG_TRCK_LOG_NUM        2
#define SEC_NUM                 2
#define DAT_NUM                 2
#define INTR_LEAV               2
#define TRCK_SKEW               2
#define SRNDR_SKEW              2
#define DSABL_PRFTCH_LEN        2
#define MIN_PRFTCH              2
#define MAX_PRFTCH              2
#define MAX_PRFTCH_CELNG        2
#define INTRFACE_ID             2
#define RDY_AEN_HLD_TIME        2
#define MAX_NTCH_NUM            2
#define ACTV_NTCH               2
#define NTCH_STRT_POS           4
#define NTCH_END_POS            4
#define NTCH_PARA_PAGE          8
#define PKT_SIZE_LEN		4
#define TRK_SIZE_LEN		4
#define AU_PAUSE_LEN		2
#define MECAT_NUM_LEN		16
#define REC_CODE_LEN		16
#define DRIVE_CAPABLE		4	
#define MAX_SPEED		2
#define NUM_VOL_LEVEL		2
#define DRIVE_BUFSIZE_LEN	2
#define CUR_SPEED		2
#define LOGICAL_BLK_ADDR        4
#define BLK_LEN                 4
#define TOC_TRK_SADDR           4   
#define TOC_DATA_LEN            2
#define MAX_TOC_DESC            100
#define MAX_TOC_DESC_DATA       (8 * 10)
#define CONFIG_DATA_LEN         4
#define CUR_PROFILE_LEN         2
#define FEATURE_CODE_LEN        2
#define MAX_FEA_DEP_DATA        400
#define PROFILE_NUM_LEN         2
#define FEATURE_MAX_NUM         40
#define PROFILE_MAX_NUM         40
#define DEP_DATA_MAX            20
#define DISC_INFO_LEN		2
#define DISC_ID_LEN		4
#define SESSION_ADDR_LEN	4
#define DISC_BC_LEN		8
#define TRK_INFO_LEN		2
#define TRK_ADDR_LEN		4
#define TRK_BLK_LEN		4
#define PERFM_DESC_LEN   	28
#define START_LBA_LEN		4
#define END_LBA_LEN		4
#define READ_SIZE_LEN		4
#define READ_TIME_LEN		4
#define WRITE_SIZE_LEN		4
#define WRITE_TIME_LEN		4
#define FMT_DESC_LEN		2
#define FMT_DESC_MAX		508
#define ISO_SECTOR_SIZE		2048
#define CD_SECTOR_FORMAT_LEN	2352
#define SYNC_FIELD_LEN		12

/*-----------------------------------
    Mode Select Parameter
-----------------------------------*/
#define DBD                     0x08
#define MD_SENSE_PAGE_CODE      0x3f
#define PAGE_CONTROL            0xc0
#define DPO                     0x10
#define FUA                     0x08
#define RELADR                  0x01
#define ALL_PARA_PAGE           0x3f
#define ALL_PARA_SIZE           124
#define DNSTY                   0x00

/*---- Block Descrptor ----*/
#define BLKNUM_LEN              3       /* length of Block number           */
#define BLKLEN_LEN              3       /* length of Block length           */
#define BLK_DSCRPT_MAX          1       /* Block descriptor max number      */

/*---- page control ----*/
#define CURRENT                 0x00    /* current parameter                */
#define CHANGABLE               0x01    /* changable parameter              */
#define DEFAULT                 0x02    /* default parameter                */
#define SAVED                   0x03    /* saved parameter                  */

/*---- page code ----*/
#define PAGE_MAX                100     /* Parameter page max size          */
#define PARA_PAGE_MAX           19      /* Parameter page max number        */
#define EXTRA_PARA_PAGE_MAX		10	 	/* Extra Parameter page max number  */

#define RWERR_RECVRY_PAGE       0x01    /* read/write error                 */
#define DECNCT_RECNCT_PAGE      0x02    /* disconnect/reconnect parameter   */
#define FORMT_PAGE              0x03    /* format parameter                 */
#define WRITE_PARAMODE_PAGE	0x05	/* write parameter mode 	    */
#define VRFYERR_RECVRY_PAGE     0x07    /* verify error recovery parameter  */
#define CACHE_CNTRL_PAGE        0x08    /* cache contorol parmeter          */
#define CNTRL_MODE_PAGE         0x0a    /* contorol mode parameter          */
#define PERPHRA_PAGE            0x09    /* peripheral parameter             */
#define MEDIA_TYPE_PAGE         0x0b    /* medium type parameter            */
#define NOTCH_PERTITION_PAGE    0x0c    /* notch/partition parameter        */
#define INFO_EXCEPTIONS_PAGE    0x1c
#define CDROM_CAPABLE_PAGE	0x2a	    /* CD-ROM Capabilities and 
					                       Mechanical Status Page Format    */

/*---- page code length ----*/
#define RWERR_RECVRY_LEN        0x0a
#define DECNCT_RECNCT_LEN       0x0e
#define FORMT_LEN               0x16
#define VRFYERR_RECVRY_LEN      0x0a
#define CACHE_CNTRL_LEN         0x0a
#define PERPHRAL_DEV_LEN        0x06
#define CNTRL_MODE_LEN          0x06
#define MEDIA_TYPE_LEN          0x06
#define NOTCH_PERTITION_LEN     0x16
#define INFO_EXCEPTIONS_LEN     0x0a
#define WRITE_PARAMODE_LEN	    0x32
#define CDROM_CAPABLE_LEN	    0x46

/*---- read/write error recovery parameter ----*/
#define AWRE                0x00    /* Automatic Write Reallocation Enable  */
#define ARRE                0x00    /* Automatic Read Reallocation Enabke   */
#define TB                  0x00    /* Trahsfer Block                       */
#define RC                  0x00    /* Read Continuous                      */
#define EER                 0x00    /* Enable Early Recovery                */
#define PER                 0x00    /* Post Error                           */
#define DTE                 0x00    /* Disable Transfer on Error            */
#define DCR                 0x01    /* Disable Correction                   */
#define R_RETRY_MAX         1
#define DAT_CRECT_LEN       0
#define HEAD_OFFSET         0
#define DAT_STROV_OFFSET    0
#define W_RETRY_MAX         1
#define RECVRY_TIME         100

/*---- Disconnect/Reconnect parameter ----*/
#define BUF_FUL_RATIO       0x80
#define BUF_EMP_RATIO       0x80
#define BUS_INACT_LIM       0x00
#define DESCNCT_TIME_LIM    0x00
#define CNCT_TIME_LIM       0x00
#define BURST_SIZE          0x00
#define DTDC                0x00

/*---- Disconnect/Reconnect parameter ----*/
#define VERFY_RETRY_MAX     1
#define VERFY_RETRY_LEN     0
#define VERFY_RCVRY_LIM     100

/*---- Write para mode parameter ----*/
#define FIX_PACKET		(1 << 5)	/* Fix Packet Type		*/
#define DEFAULT_TRK_MODE	5		/* Default Track Mode		*/
#define DEFAULT_DATABLOCK_TYPE	8		/* Default Data Block Type	*/
#define PKT_SIZE		0x20		/* Packet Size			*/
#define AUDIO_PAUSE_LEN		150		/* Default Audio Pause Length	*/
/*---- CDROM capabilities parameter  ----*/
#define CD_R_RD			(1 << 0)	/* CD-R Read			*/
#define CD_RW_RD		(1 << 1)	/* CD-RW Read			*/
#define METHOD_2_RD		(1 << 2)	/* METHOD 2			*/
#define DVD_ROM_RD		(1 << 3)	/* DVD-ROM Read			*/
#define DVD_R_RD		(1 << 4)	/* DVD-R Read			*/
#define DVD_RAM_RD		(1 << 5)	/* DVD-RAM Read			*/
#define CD_R_WR			(1 << 0)	/* CD-R Write			*/
#define CD_RW_WR		(1 << 1)	/* CD-RW Write			*/
#define TEST_WRITE_WR		(1 << 2)	/* Test Write			*/
#define DVD_R_WR		(1 << 4)	/* DVD-R Write			*/
#define DVD_RAM_WR		(1 << 5)	/* DVD-RAM Write		*/
#define	DRIVE_CAPABLE_B0	0xf1	/* MutliSession ; Mode 2 form 2 ; 
					   Mode2 form 1 ; AudioPlay		*/
#define DRIVE_CAPABLE_B1	0x77	/* UPC ; ISRC ; C2 ; R-W Supported ; 
					   DAAccu; CD DA              		*/
#define DRIVE_CAPABLE_B2	0x49	/* Loading Mechanism Type ; 
					   Prevent Jump ; Lock State ; Lock	*/	
#define	DRIVE_CAPABLE_B3	0x23	/* Separate Channel Mute and Volume	*/
#define	MAX_SPEED_SUPPORTED	0x108a	/* Max Speed supported			*/	
#define NUM_VOLUME_VALUE	0x0100	/* Number of volume values supported	*/	
#define DRIVE_BUF_SIZE		0x0800	/* Buffer Size supported by Drive	*/	
#define DIGITAL_OUTPUT		0x10	/* Length				*/

/*-----------------------------------
    Get Configuration
-----------------------------------*/
/*---- Feature Code ----*/
#define FEATURE_PROFILE_LIST            0x0000
#define FEATURE_CORE                    0x0001                    
#define FEATURE_MORPHING                0x0002
#define FEATURE_REMOVABLE_MEDIUM        0x0003
#define FEATURE_WRITE_PROTECT           0x0004
#define FEATURE_RANDOM_READABLE         0x0010
#define FEATURE_MULTIREAD               0x001D
#define FEATURE_CD_READ                 0x001E
#define FEATURE_DVD_READ                0x001F
#define FEATURE_RANDOM_WRITABLE         0x0020
#define FEATURE_INCRE_STREAM_WRITABLE   0x0021
#define FEATURE_SECTOR_ERASABLE         0x0022
#define FEATURE_FORMATTABLE             0x0023
#define FEATURE_HW_DEFECT_MANAGE        0x0024
#define FEATURE_RESTRICTED_OVERWTITE    0x0026
#define FEATURE_CDRW_CAV_WRITE          0x0027
#define FEATURE_DVD_RWP                 0x002A
#define FEATURE_DVD_RP                  0x002B
#define FEATURE_RG_RESTRICTED_OVERWRITE 0x002C
#define FEATURE_CD_TRACK_AT_ONCE        0x002D
#define FEATURE_CD_MASTERING            0x002E
#define FEATURE_DVD_RM_RWM_WRITE        0x002F
#define FEATURE_LAYER_JUMP_RECORDING    0x0033
#define FEATURE_0x34                    0x0034
#define FEATURE_CDRW_MEDIAWRITE_SUPPORT 0x0037
#define FEATURE_0x3A                    0x003A
#define FEATURE_DVD_RP_DUAL_LAYER       0x003B
#define FEATURE_POWER_MANAGEMENT        0x0100        
#define FEATURE_SMART                   0x0101
#define FEATURE_CD_AUDIO_EXT_PLAY       0x0103
#define FEATURE_MICROCODE_UPGRADE       0x0104
#define FEATURE_TIMEOUT                 0x0105
#define FEATURE_DVD_CSS                 0x0106
#define FEATURE_REAL_TIME_STREAMING     0x0107
#define FEATURE_DRIVE_SN                0x0108
#define FEATURE_DCB                     0x010A
#define FEATURE_DVD_CPRM                0x010B

/*---- Profile List ----*/
#define PROFILE_OBSOLETE                0x0001
#define PROFILE_REMOVABLE_DISK          0x0002
#define PROFILE_MO_ERASABLE             0x0003
#define PROFILE_MO_WRITE_ONCE           0x0004
#define PROFILE_CD_ROM                  0x0008
#define PROFILE_CD_R                    0x0009
#define PROFILE_CD_RW                   0x000A
#define PROFILE_DVD_ROM                 0x0010
#define PROFILE_DVD_R_S                 0x0011
#define PROFILE_DVD_RAM                 0x0012  
#define PROFILE_DVD_RW_R                0x0013
#define PROFILE_DVD_RW_S                0x0014
#define PROFILE_DVDR_DUAL_SREC          0x0015
#define PROFILE_DVDR_DUAL_JREC          0x0016
#define PROFILE_DVD_RWM_DUAL_LAYER      0x0017
#define PROFILE_DVD_RW                  0x001A
#define PROFILE_DVD_R                   0x001B
#define PROFILE_DVD_RWP_DUAL_LAYER      0x002A
#define PROFILE_DVDR_DUAL_LAYER         0x002B
#define PROFILE_UNDEFINED               0xFFFF

#define FEATURE_CURRENT         (0x01 << 0)
#define FEATURE_PERSISTENT      (0x01 << 1)
#define FEATURE_VERSION0        (0x00 << 2)
#define FEATURE_VERSION1        (0x01 << 2)
#define FEATURE_VERSION2        (0x02 << 2)
#define FEATURE_VERSION3        (0x03 << 2)
#define FEATURE_VERSION5        (0x05 << 2)

#define NUM_LINK_SIZE   4
#define NUM_PAD_BYTES   0
/*---- Additional Length ----*/ 
#define ADD_LENGTH_FEATURE_0x01         0x08
#define ADD_LENGTH_FEATURE_0x02         0x04
#define ADD_LENGTH_FEATURE_0x03         0x04
#define ADD_LENGTH_FEATURE_0x04         0x04
#define ADD_LENGTH_FEATURE_0x10         0x08
#define ADD_LENGTH_FEATURE_0x1D         0x00
#define ADD_LENGTH_FEATURE_0x1E         0x04
#define ADD_LENGTH_FEATURE_0x1F         0x04
#define ADD_LENGTH_FEATURE_0x20         0x0C
#define ADD_LENGTH_FEATURE_0x21         (4 + NUM_LINK_SIZE + NUM_PAD_BYTES) 
#define ADD_LENGTH_FEATURE_0x23         0x08
#define ADD_LENGTH_FEATURE_0x24         0x04
#define ADD_LENGTH_FEATURE_0x26         0x00
#define ADD_LENGTH_FEATURE_0x27         0x04
#define ADD_LENGTH_FEATURE_0x2A         0x04
#define ADD_LENGTH_FEATURE_0x2B         0x04
#define ADD_LENGTH_FEATURE_0x2C         0x04
#define ADD_LENGTH_FEATURE_0x2D         0x04
#define ADD_LENGTH_FEATURE_0x2E         0x04
#define ADD_LENGTH_FEATURE_0x2F         0x04
#define ADD_LENGTH_FEATURE_0x33         0x08
#define ADD_LENGTH_FEATURE_0x34         0x04
#define ADD_LENGTH_FEATURE_0x37         0x04
#define ADD_LENGTH_FEATURE_0x3A         0x04
#define ADD_LENGTH_FEATURE_0x3B         0x04
#define ADD_LENGTH_FEATURE_0x100        0x00
#define ADD_LENGTH_FEATURE_0x101        0x04
#define ADD_LENGTH_FEATURE_0x103        0x04
#define ADD_LENGTH_FEATURE_0x104        0x04
#define ADD_LENGTH_FEATURE_0x105        0x04
#define ADD_LENGTH_FEATURE_0x106        0x04
#define ADD_LENGTH_FEATURE_0x107        0x04
#define ADD_LENGTH_FEATURE_0x108        0x14
#define ADD_LENGTH_FEATURE_0x10A        0x0C
#define ADD_LENGTH_FEATURE_0x10B        0x04

/*---- Feature Dependent ----*/
#define FEATURE_0x01_STANDARD_ATAPI     0x00000007
#define FEATURE_0x01_DBE                1
#define FEATURE_0x03_LOADING_TYPE       (0x2 << 5)
#define FEATURE_0x03_EJECT              (1 << 3)
#define FEATURE_0x03_LOCK               1
#define FEATURE_0x04_SPWP               (1 << 1)
#define FEATURE_0x10_BLOCKING           0x01
#define FEATURE_0x10_PP                 1
#define FEATURE_0x1E_C2_FLAG            (1 << 1)
#define FEATURE_0x1E_CD_TEXT            1
#define FEATURE_0x1F_DUAL_R             1
#define FEATURE_0x20_PP                 1
#define FEATURE_0x24_SSA                (1 << 7)
#define FEATURE_0x2A_WRITE              1
#define FEATURE_0x2B_WRITE              1
#define FEATURE_0x2C_INTERMEDIATE       (1 << 1)
#define FEATURE_0x2C_BLANK              1
#define FEATURE_0x2D_RW_RAW             (1 << 4)
#define FEATURE_0x2D_RW_PACK            (1 << 3)
#define FEATURE_0x2D_TEST_WRITE         (1 << 2)
#define FEATURE_0x2D_CD_RW              (1 << 1)
#define FEATURE_0x2D_RW_SUB_CODE        1
#define FEATURE_0x37_SUBTYPE0           1
#define FEATURE_0x37_SUBTYPE1           (1 << 1)
#define FEATURE_0x37_SUBTYPE2           (1 << 2)
#define FEATURE_0x37_SUBTYPE3           (1 << 3)
#define FEATURE_0x3B_WRITE              1
#define FEATURE_0x106_CSS_VERSION       0x01
#define FEATURE_0x107_SCS               (1 << 3)
#define FEATURE_0x107_MP2A              (1 << 2) 
#define FEATURE_0x107_WSPD              (1 << 1)         
#define FEATURE_0x107_SW                1
#define FEATURE_0x10B_CPRM_VERSION      0x01

/*-----------------------------------
    Read TOC
-----------------------------------*/
/*---- TOC Format ----*/
#define RSP_FMT_FORMTTED_TOC    0x00
#define RSP_FMT_MULTI_SESSION   0x01
#define RSP_FMT_RAW_TOC         0x02
#define RSP_FMT_PMA             0x03
#define RSP_FMT_CD_TEXT         0x05

/*---- Storage Format ----*/
#define STRG_FORMAT_UDF         0x00
#define STRG_FORMAT_CDFS        0x01
#define STRG_FORMAT_OTHERS      0x02

/*-----------------------------------
    Track Number in CDROM Media (Single Track)
-----------------------------------*/
#define FIRST_TRACK_NUMBER	1
#define LAST_TRACK_NUMBER	1
#define NUMBER_OF_SESSIONS	1
#define DEFAULT_SESSION_NUMBER	1

enum {
	/* The sector to check if the file system is ISO9660 */
        ISO9660_FIRST_CHECK_SECTOR = 64,
	/* Start position of Standard Id */  
        ISO9660_STARTBP_STDID = 1,
	/* Start position of total number of sectors (BigEndian) */      
        ISO9660_STARTBP_SECNUM = 84,   
};

/*---- RT field ----*/
#define RT_ALL_FEATURE          0x00
#define RT_CURRENT_FEATURE      0x01
#define RT_SFN_FEATURE          0x02

/*---- Media Type ----*/
#define MEDIA_CD_R              0x00
#define MEDIA_CD_RW             0x01
#define MEDIA_OTHERS            0x02

/*---- Mode Parameter Header ----*/
typedef struct {
    UCHAR   ModeParaLen[MODE_PARA_LEN]; /* Mode parameter length            */
    UCHAR   MediaType;                  /* Medium type                      */
    UCHAR   DevPara;                    /* Device specific parameter        */
    UCHAR   Reserved[2];                /* Rserved area                     */
    UCHAR   BlkDscrptLen[BLK_DSCRP_LEN];/* Block descriptor length          */
} MODE_PARA_HDR10;

typedef struct {
    UCHAR   FeatureCode[FEATURE_CODE_LEN];
                                        /* Feature Code                     */
    UCHAR   VerPerCur;                  /* Version/Persistent/Current       */
    UCHAR   AddLen;                     /* Additional Length                */
    UCHAR*  FeatureDepData;             /* Feature Dependent Data           */                                           
} FEATURE_DESCRIPTOR;

typedef struct {
    UCHAR   DataLen[CONFIG_DATA_LEN];   /* Data Length                      */
    UCHAR   Reserved1;                  /* Reserved                         */
    UCHAR   Reserved2;                  /* Reserved                         */
    UCHAR   CurProfile[CUR_PROFILE_LEN];/* Current Profile                  */   
    UCHAR   FeatureDesc[MAX_FEA_DEP_DATA];
                                        /* Feature Descriptors              */
} FEATURES;

typedef struct {
    UCHAR   ProfileNum[PROFILE_NUM_LEN];
    UCHAR   Current;
    UCHAR   Reserved;    
} PROFILE_DESCRIPTOR;

/*---- Parameter Page ----*/
typedef struct {
    UCHAR   PageCode;                   /* Code of the Page                 */
    UCHAR   PageLen;                    /* Length of the Page               */
    UCHAR   ModePara[PAGE_MAX];
} PARAMETER_PAGE;

/*---- CDROM Capabilites Prameter ----*/
typedef struct {
    UCHAR   PageCode;                   /* Code of the Page                 */
    UCHAR   PageLen;                    /* Length of the Page               */
    UCHAR   RDiscCapable;           	/* Read Disc Capabilities	    */
    UCHAR   WDiscCapable;		/* Write Disc Capabilities	    */
    UCHAR   DriveCapable[DRIVE_CAPABLE];/* Individual Drive capabilities    */			
    UCHAR   MaxSpeed[MAX_SPEED];	/* Max speed supported		    */
    UCHAR   NumVolLevel[NUM_VOL_LEVEL];	/* Num of volume level supported    */
    UCHAR   BufSize[DRIVE_BUFSIZE_LEN];	/* Buffer size supported by drive   */
    UCHAR   CurSpeed[CUR_SPEED];	/* Current speed selected	    */
    UCHAR   Reserved1;			/* Reserved			    */
    UCHAR   DigitalOuput;		/* Digital output of the drive	    */
    UCHAR   Reserved2[54];			/* Reserved			    */
    //UCHAR   Reserved3;			/* Reserved			    */    
} CDROM_CAPABILITIES;

/*---- Write Prameter ----*/
typedef struct {
    UCHAR   PageCode;                   /* Code of the Page                 */
    UCHAR   PageLen;                    /* Length of the Page               */
    UCHAR   BufeLsvTWrWtype;           	/* BUFE;LS_V;Test Write;Write Type  */
    UCHAR   BorFPCpTrkmode;		/* Border;FP;Copy;Track Mode	    */
    UCHAR   DataBlockType;		/* Data block type		    */
    UCHAR   LinkSize;			/* Link size			    */
    UCHAR   Reserved1;			/* Reserved			    */
    UCHAR   HostAppCode;		/* Host application code	    */
    UCHAR   SessionFmt;			/* Session format		    */
    UCHAR   Reserved2;			/* Reserved			    */
    UCHAR   PktSize[PKT_SIZE_LEN];	/* Packet size			    */
    UCHAR   AudioPauseLen[AU_PAUSE_LEN];/* Audio pause length		    */
    UCHAR   MediaCatNum[MECAT_NUM_LEN];	/* Media catalog number		    */
    UCHAR   IntStdRecCode[REC_CODE_LEN];/* International standard rec code  */
    UCHAR   SubHdrB0;			/* Sub-header byte 0		    */
    UCHAR   SubHdrB1;			/* Sub-header byte 1		    */
    UCHAR   SubHdrB2;			/* Sub-header byte 2		    */
    UCHAR   SubHdrB3;			/* Sub-header byte 3		    */    
} WRITE_PARAMETER;

/*---- Read TOC Parameter ----*/
typedef struct {
    UCHAR   DataLen[TOC_DATA_LEN];      /* Data length                      */
    UCHAR   FstTrkFd;                   /* First track field                */
    UCHAR   LstTrkFd;                   /* Last track field                 */
    UCHAR   DescData[MAX_TOC_DESC_DATA];   
                                        /* TOC track descriptor(s)          */
} TOC_RSP_DATA;

typedef struct {
    UCHAR   Reserved1;                  /* Reserved area                    */
    UCHAR   AdrCtl;                     /* Addr & Control                   */            
    UCHAR   TrkNum;                     /* Track Number                     */
    UCHAR   Reserved2;                  /* Reserved area                    */
    UCHAR   TrkSAddr[TOC_TRK_SADDR];    /* Track Start Address              */        
} TRK_DESC_FMT_TOC;

typedef struct {
    UCHAR   SessionNum;                 /* Session Number                   */
    UCHAR   AdrCtl;                     /* Addr & Control                   */            
    UCHAR   TNO;                        /* TNO Field                        */
    UCHAR   POINT;                      /* POINT Field                      */
    UCHAR   Min;                        /* Min Field                        */
    UCHAR   Sec;                        /* Sec Field                        */
    UCHAR   Frame;                      /* Frame Field                      */
    UCHAR   Zero;                       /* Zero Field                       */
    UCHAR   PMIN;                       /* PMIN Field                       */
    UCHAR   PSEC;                       /* PSEC Field                       */
    UCHAR   PFRAME;                     /* PFRAME Field                     */
} TRK_DESC_RAW_TOC;

/* Define Slave Storage Class LUN structure.  */

typedef struct UX_SLAVE_CLASS_STORAGE_LUN_STRUCT
{
    ULONG           ux_slave_class_storage_media_last_lba;
    ULONG           ux_slave_class_storage_media_block_length;
    ULONG           ux_slave_class_storage_media_type;
    ULONG           ux_slave_class_storage_media_removable_flag;
    ULONG           ux_slave_class_storage_media_id;
    ULONG           ux_slave_class_storage_scsi_tag;
    UCHAR           ux_slave_class_storage_request_sense_key;
    UCHAR           ux_slave_class_storage_request_code;
    UCHAR           ux_slave_class_storage_request_code_qualifier;
    UCHAR           ux_slave_class_storage_write_protect;
    UINT            (*ux_slave_class_storage_media_read)(VOID *storage, ULONG lun, UCHAR * data_pointer, ULONG number_blocks, ULONG lba, ULONG *media_status);
    UINT            (*ux_slave_class_storage_media_write)(VOID *storage, ULONG lun, UCHAR * data_pointer, ULONG number_blocks, ULONG lba, ULONG *media_status);
    UINT            (*ux_slave_class_storage_media_status)(VOID *storage, ULONG lun, ULONG media_id, ULONG *media_status);
    UCHAR           media_unload;
} UX_SLAVE_CLASS_STORAGE_LUN;

typedef struct UDC_MSC_WRITE_INFO_STRUCT
{
	ULONG lun;
	UCHAR *data;
	ULONG sectors;
	ULONG sector_start;
	ULONG result;
} UDC_MSC_WRITE_INFO;

typedef struct UDC_MSC_READ_SEND_INFO_STRUCT
{
	ULONG lun;
	UCHAR *data;
	ULONG size;
	ULONG result;
	UX_SLAVE_ENDPOINT *ux_endpoint_in;
} UDC_MSC_READ_SEND_INFO;

#define UDC_MSC_WRITE_START 0x10
#define UDC_MSC_WRITE_DONE  0x01

#define UDC_MSC_READ_SEND_START 0x10
#define UDC_MSC_READ_SEND_DONE  0x01

#define UDC_MSC_CACHE_BLOCK_DATA_NUM		2	// at least 2
#define UDC_MSC_CACHE_BLOCK_DATA_SIZE		(128 * 512)
#define UDC_MSC_CACHE_MAX_BLOCK_NUMBER		(UDC_MSC_CACHE_BLOCK_DATA_SIZE/512)

typedef struct UDC_MSC_READ_CACHE_INFO_STRUCT {
    int block_start;
    int block_end;
    UINT8 cached;
    UINT8 cached_data_index;
    UCHAR  *data[UDC_MSC_CACHE_BLOCK_DATA_NUM];
	UCHAR  *data_raw[UDC_MSC_CACHE_BLOCK_DATA_NUM];
} UDC_MSC_READ_CACHE_INFO;

/* Define Slave Storage Class structure.  */

typedef struct UX_SLAVE_CLASS_STORAGE_STRUCT
{
	UX_SLAVE_INTERFACE			*ux_slave_class_storage_interface;
	ULONG						ux_slave_class_storage_number_lun;
	ULONG						ux_slave_class_storage_null_lun;
	UX_SLAVE_CLASS_STORAGE_LUN  ux_slave_class_storage_lun[UX_MAX_SLAVE_LUN];
	// write thread
	UDC_MSC_WRITE_INFO			write_info;
	TX_THREAD					write_thread;
	VOID                        *write_thread_stack;
	UCHAR                       *write_buffer[UDC_MSC_CACHE_BLOCK_DATA_NUM];
	ULONG                       write_buffer_size;
	TX_EVENT_FLAGS_GROUP		write_flag;
	ULONG                       buffer_index;
	// read thread
	UDC_MSC_READ_SEND_INFO      read_info;
	TX_THREAD					read_thread;
	VOID                        *read_thread_stack;
	TX_EVENT_FLAGS_GROUP		read_flag;
	UCHAR                       *read_buffer;
	ULONG                       read_buffer_size;
	UDC_MSC_READ_CACHE_INFO     read_cache_info;
	TX_MUTEX                    mutex;

} UX_SLAVE_CLASS_STORAGE;

/* Define Slave Storage Class Calling Parameter structure */

typedef struct UX_SLAVE_CLASS_STORAGE_PARAMETER_STRUCT
{
    ULONG                       ux_slave_class_storage_parameter_number_lun;
    ULONG                       ux_slave_class_storage_parameter_null_lun;
    UX_SLAVE_CLASS_STORAGE_LUN  ux_slave_class_storage_parameter_lun[UX_MAX_SLAVE_LUN];

} UX_SLAVE_CLASS_STORAGE_PARAMETER;

/* Define Device Storage Class prototypes.  */

UINT    _ux_device_class_storage_initialize(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_storage_reset(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_storage_activate(UX_SLAVE_CLASS_COMMAND *command);
VOID    _ux_device_class_storage_control_request(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_storage_csw_send(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in, UCHAR csw_status);
UINT    _ux_device_class_storage_deactivate(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_storage_entry(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_storage_format(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
void    _ux_device_class_storage_InitFeatureDescs(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun);
UINT    _ux_device_class_storage_get_configuration(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
void    _ux_device_class_storage_InitMmcPara(void);
UINT    _ux_device_class_storage_inquiry(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
UINT    _ux_device_class_storage_mode_select(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
UINT    _ux_device_class_storage_mode_sense(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
UINT    _ux_device_class_storage_prevent_allow_media_removal(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, 
                    UX_SLAVE_ENDPOINT *endpoint_in, UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
UINT    _ux_device_class_storage_read(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb, UCHAR scsi_command);
UINT    _ux_device_class_storage_read_capacity(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
UINT    _ux_device_class_storage_read_format_capacity(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
UINT    _ux_device_class_storage_read_toc(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                                            UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
UINT    _ux_device_class_storage_request_sense(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
UINT    _ux_device_class_storage_start_stop(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
UINT    _ux_device_class_storage_test_ready(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
VOID    _ux_device_class_storage_thread(ULONG storage_instance);
UINT    _ux_device_class_storage_verify(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);
UINT    _ux_device_class_storage_write(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, UX_SLAVE_ENDPOINT *endpoint_in,
                    UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb, UCHAR scsi_command);
UINT32 AmbaUSB_Class_Msc_SetKernelInfo(UX_SLAVE_CLASS_STORAGE* info);
UINT  _ux_device_class_storage_read_set_cache_block_number(UINT blocks);
UINT  _ux_device_class_storage_read_get_cache_block_number(void);
UINT  _ux_device_class_storage_read_cd(UX_SLAVE_CLASS_STORAGE *storage, ULONG lun, 
                                            UX_SLAVE_ENDPOINT *endpoint_in,
                                            UX_SLAVE_ENDPOINT *endpoint_out, UCHAR * cbwcb);

#define UDC_MSC_CBW_PROCESS_DONE         0 // Callback has processed the CBW
#define UDC_MSC_CBW_PROCESS_CONTINUE     1 // Callback has not processed the CBW

// MSC CSW Status
#define UDC_MSC_CSW_STS_GOOD        0x00 // Good Status
#define UDC_MSC_CSW_STS_CMD_FAILED  0x01 // Command Failed
#define UDC_MSC_CSW_STS_PHASE_ERR   0x02 // Phase Error
#define UDC_MSC_CSW_STS_SENDED      0xff // status data has sended
#define UDC_MSC_CSW_STS_USB_ERR     0xfe // usb send/receive error

typedef UINT32 (*UDC_MSC_CBK_PROCESSCBW__)(UINT8 *cbwcb, UINT32 cbwcb_length, UINT8 *status);
UINT32 udc_msc_setcbk_processcbw(UDC_MSC_CBK_PROCESSCBW__ pfnCallBackFunc );
UINT32 udc_msc_recv_bulkout_data(UINT8 *data, UINT32 length, UINT32 *bytes_read);
UINT32 udc_msc_send_bulkin_data(UINT8 *data, UINT32 length);
VOID   udc_msc_write_thread(ULONG arg);
VOID   udc_msc_read_thread(ULONG arg);
UINT   udc_msc_enable_write_speed_improvement(UINT value);
UINT   udc_msc_get_write_speed_improvement(void);
VOID   udc_msc_mutex_get(UX_SLAVE_CLASS_STORAGE *storage, UINT32 option);
VOID   udc_msc_mutex_put(UX_SLAVE_CLASS_STORAGE *storage);
struct UX_SLAVE_CLASS_STORAGE_STRUCT *udc_msc_get_context(VOID);
VOID   udc_msc_clear_context(VOID);

#endif
