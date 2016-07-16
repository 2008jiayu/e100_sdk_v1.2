/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaB5_Delimiter.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for B5 Delimiter Control APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_B5_DELIMITER_H_
#define _AMBA_B5_DELIMITER_H_

typedef enum _AMBA_B5_DELIMITER_PIXEL_DEPTH_e_ {
    PIXEL_DEPTH_8_BIT,
    PIXEL_DEPTH_10_BIT,
    PIXEL_DEPTH_12_BIT,
    PIXEL_DEPTH_14_BIT
} AMBA_B5_DELIMITER_PIXEL_DEPTH_e;

typedef enum _AMBA_B5_DELIMITER_LANE_USAGE_e_ {
    LANE0_ONLY,
    LANE0_AND_LANE1,
    LANE0_LANE1_AND_LANE2
} AMBA_B5_DELIMITER_LANE_USAGE_e;

typedef struct _AMBA_B5_DELIMITER_PACKET_LENGTH_s_ {
    UINT16 PacketMinByte;
    UINT16 PacketMaxByte;
} AMBA_B5_DELIMITER_PACKET_LENGTH_s;

typedef struct _AMBA_B5_DELIMITER_CONFIG_s_ {
    AMBA_B5_DELIMITER_PIXEL_DEPTH_e     PixelDepth;
    AMBA_B5_DELIMITER_LANE_USAGE_e      LaneCfg;
    AMBA_B5_DELIMITER_PACKET_LENGTH_s   PacketLength;
    UINT8                               PixelBufTh;
    UINT32                              IntStatPacketPeriod;                
} AMBA_B5_DELIMITER_CONFIG_s;

/*---------------------------------------------------------------------------*\
 * Defined in AmbaB5_Delimiter.c
\*---------------------------------------------------------------------------*/
int AmbaB5_DelimiterInit(AMBA_B5_CHIP_ID_u ChipID);
int AmbaB5_DelimiterIntPacketEnable(AMBA_B5_CHIP_ID_u ChipID);
int AmbaB5_DelimiterPixelDataPacketEnable(AMBA_B5_CHIP_ID_u ChipID);
int AmbaB5_DelimiterSetPixelDepth(AMBA_B5_CHIP_ID_u ChipID, AMBA_B5_DELIMITER_PIXEL_DEPTH_e PixelDepth);
int AmbaB5_DelimiterLaneCfg(AMBA_B5_CHIP_ID_u ChipID, AMBA_B5_DELIMITER_LANE_USAGE_e Lane);
int AmbaB5_DelimiterSetPixelBufTh(AMBA_B5_CHIP_ID_u ChipID, UINT8 Threshold);
int AmbaB5_DelimiterSetPacketByte(AMBA_B5_CHIP_ID_u ChipID, UINT16 MinimalByte, UINT16 MaximalByte);
int AmbaB5_DelimiterSetIntStatPktPeriod(AMBA_B5_CHIP_ID_u ChipID, UINT32 Period);
int AmbaB5_DelimiterResetSlvsecWaitCnt(AMBA_B5_CHIP_ID_u ChipID);

#endif  /* _AMBA_B5_DELIMITER_H_ */
