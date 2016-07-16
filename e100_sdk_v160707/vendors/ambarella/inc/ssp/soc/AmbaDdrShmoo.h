/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaDdrShmoo.h
 *
 *  @Copyright      :: Copyright (C) 2014 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella DDR Shmoo function.
\*-------------------------------------------------------------------------------------------------------------------*/

#ifndef _AMBA_DDR_SHMOO_H_
#define _AMBA_DDR_SHMOO_H_

typedef struct _AMBA_DDR_SHMOO_CONFIG_s_ {
    int     Priority;                   /* Priority */
    void    *pStack;                    /* pStackBase */
    int     StackByteSize;              /* StackByteSize */

    int     RecordTime;                 /* Record time in second */
    void    (*VideoRecordStart)(void);  /* pointer to the function of start Video Record */
    void    (*VideoRecordStop)(void);   /* pointer to the function of stop Video Record */
    void    (*VideoPlayback)(void);     /* pointer to the function of playback Video */
    void    (*VideoFileDelete)(void);   /* pointer to the function of delete the Video file on SD card */
} AMBA_DDR_SHMOO_CONFIG_s;

int AmbaDdrShmooTask_Start(AMBA_DDR_SHMOO_CONFIG_s *pDdrShmooConfig);

#endif  /* _AMBA_DDR_SHMOO_H_ */
