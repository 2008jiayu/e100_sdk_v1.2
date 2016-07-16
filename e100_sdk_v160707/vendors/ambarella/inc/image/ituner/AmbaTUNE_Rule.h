/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaTUNE_Rule.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Rules of The Ambarella Img Tuning Tools
\*-------------------------------------------------------------------------------------------------------------------*/
/*!
 * @addtogroup ParserHdlr
 * @{
 *      @addtogroup Rule
 *      @{
 */
#ifndef _AMBA_TEXT_RULE_H_
#define _AMBA_TEXT_RULE_H_
/*******************************************************************************
 * Header include
 ******************************************************************************/
#define SKIP_VALID_CHECK 0xFFFFFFFFFFFFFFFFULL
typedef struct {
    REG_s * RegList;
    UINT8 RegCount;
} Rule_Info_t;

int TUNE_Rule_Get_Info(Rule_Info_t * RuleInfo);
const char* TUNE_Rule_Lookup_Tuning_Mode_Str(TUNING_MODE_e Tuning_Mode);
const char* TUNE_Rule_Lookup_Tuning_Mode_Ext_Str(TUNING_MODE_EXT_e Tuning_Mode_Ext);
TUNING_MODE_EXT_e TUNE_Rule_Lookup_Tuning_Mode_Ext(const char* Key);
TUNING_MODE_e TUNE_Rule_Lookup_Tuning_Mode(const char* Key);
#endif /* _AMBA_TEXT_HDLR_H_ */
/*!
 *      @} end of addtogroup Rule
 * @} end of addtogroup ParserHdlr
 */
