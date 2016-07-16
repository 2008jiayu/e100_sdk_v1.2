/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaAudio_CODEC.c
 *
 *  @Copyright      :: Copyright (C) 2013 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Ambarella AD/DA CODEC driver APIs
\*-------------------------------------------------------------------------------------------------------------------*/

#include "AmbaDataType.h"
#include "AmbaAudio_CODEC.h"

AMBA_AUDIO_CODEC_OBJ_s *pAmbaAudioCodecObj[AMBA_NUM_AUDIO_CODEC] = {
    [AMBA_AUDIO_CODEC_0] = 0,
};
