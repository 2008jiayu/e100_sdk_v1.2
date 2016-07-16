/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaImg_AntiFlicker.h
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella CorpoRation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, Photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella CorpoRation.
 *
 *  @Description    :: Definiton of AntiFlicker
 *
\*-------------------------------------------------------------------------------------------------------------------*/
#ifndef __AMBA_IMG_ANTIFLICKER_H__
#define __AMBA_IMG_ANTIFLICKER_H__


/**
* Flicker detection status information
*/
typedef struct _FLICKER_DETECT_STATUS_s {
    UINT8 Enable;
#define ENABLE    1
#define DISABLE    0
    UINT8 Running;
#define RUNNING    1
#define STOP    0
    UINT8 FlickerHz;
    UINT8 Reserved;
} FLICKER_DETECT_STATUS_s;

/**
* Flicker detection entry function
*
* @param [in] Enable 1-enable, 0-disable flicker detection
* @param [in] Init   Init flg, 1-Init , 0-Not to init
*/
void Img_Flicker_Detection(UINT8 Enable, UINT8 Init);

/**
* Flicker detection init function
*/
void Img_Flicker_Detection_Init(void);
/**
* Flicker detection enable function
* @param [in] Enable 1-enable, 0-disable
*/
void Img_Flicker_Detection_Enable(UINT8 Enable);
/**
* Get flicker detection status
* @param [in] fDetectStatus pointer to flicker dection status
*/
void Img_Get_Flicker_Detection_Status(FLICKER_DETECT_STATUS_s *fDetectStatus);

#endif

