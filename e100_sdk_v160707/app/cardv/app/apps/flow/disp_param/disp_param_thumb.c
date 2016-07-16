/**
 * @file src/app/apps/flow/disp_param/connectedcam/disp_param_thumb.c
 *
 * Thumbnail Scene display parameters
 *
 * History:
 *    2013/12/05 - [Martin Lai] created file
 *
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents (¡°Software¡±) are protected by intellectual property rights
 * including, without limitation, U.S. and/or foreign copyrights.  This Software is also the
 * confidential and proprietary information of Ambarella, Inc. and its licensors.  You may
 * not use, reproduce, disclose, distribute, modify, or otherwise prepare derivative
 * works of this Software or any portion thereof except pursuant to a signed license
 * agreement or nondisclosure agreement with Ambarella, Inc. or its authorized
 * affiliates.	In the absence of such an agreement, you agree to promptly notify and
 * return this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-
 * INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR
 * MALFUNCTION; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <apps/flow/disp_param/disp_param_thumb.h>

/*************************************************************************
 * Motion thumbnail mode (fish-eye: APP_THUMB_MOTION) scene settings
 ************************************************************************/
APPLIB_STILL_DISP_DESC_s basicthm_disp_dchan_3x2_h_normal[6] = {
    { {100,  2418,  3200,  3328}, 128,  1}, // index 0    
    { {3400,  2418,  3200,  3328}, 128,  1}, // index 1    
    { {6700,  2418,  3200,  3328}, 128,  1}, // index 2    
    { {100,  5210,  3200,  3328}, 128,  1}, // index 3    
    { {3400,  5210,  3200,  3328}, 128,  1}, // index 4    
    { {6700,  5210,  3200,  3328}, 128,  1}  // index 5
};

APPLIB_STILL_DISP_DESC_s basicthm_disp_dchan_3x2_h_focus[6] = {

    { {100,  2418,  3200,  3328}, 128,  1}, // index 0    
    { {3400,  2418,  3200,  3328}, 128,  1}, // index 1    
    { {6700,  2418,  3200,  3328}, 128,  1}, // index 2    
    { {100,  5210,  3200,  3328}, 128,  1}, // index 3    
    { {3400,  5210,  3200,  3328}, 128,  1}, // index 4    
    { {6700,  5210,  3200,  3328}, 128,  1}  // index 5
};


APPLIB_STILL_DISP_DESC_s basicthm_disp_fchan_3x2_h[6] = {
    { {1000,  2541,  2365,  2459}, 128,  1}, // index 0
    { {3813,  2541,  2365,  2459}, 128,  1}, // index 1
    { {6625,  2541,  2365,  2459}, 128,  1}, // index 2
    { {1000,  5333,  2365,  2459}, 128,  1}, // index 3
    { {3813,  5333,  2365,  2459}, 128,  1}, // index 4
    { {6625,  5333,  2365,  2459}, 128,  1}  // index 5
};


