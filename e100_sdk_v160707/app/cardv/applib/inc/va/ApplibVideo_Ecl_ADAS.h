/**
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
#ifndef APPLIB_VIDEO_ECL_ADAS_H_
#define APPLIB_VIDEO_ECL_ADAS_H_

/**
 * @addtogroup ApplibVideo_ECL_ADAS
 * @ingroup ADAS
 * @{
 */
 
#include <gps.h>
#include <applib.h>
#include <recorder/Encode.h>

__BEGIN_C_PROTO__

extern void AppLibVideo_Ecl_ADAS_Init(int v_ratio);

extern int AppLibVideo_Ecl_ADAS_Enable(void);
extern void  AppLibVideo_ECL_ADAS_Set_Fcwd_ttc(unsigned int ttc_t);
extern void AppLibVideo_Ecl_ADAS_Disable(void);

extern void AppLibVideo_Set_Adas_Ldws_OnOff(int ldws_mode);
extern void AppLibVideo_Set_Adas_Fcws_OnOff(int fcws_mode);
extern void AppLibVideo_Set_Adas_Hmws_OnOff(int hmws_mode);
extern void AppLibVideo_Set_Adas_Fcmrs_OnOff(int fcmrs_mode);
extern void AppLibVideo_Set_Adas_Sen_Level(int sen_level);

extern void  AppLibVideo_Set_Radar_Calibration_Mode(int mode_enable);
extern void  AppLibVideo_Set_Radar_Offset(float radar_offset);
extern float AppLibVideo_Get_Radar_Offset(void);
extern float AppLibVideo_Get_Radar_Offset_e(void);
K_RADAR_OBJ_INFO* AppLibVideo_Ecl_ADAS_GetObjData(void);
void AppLibVideo_Emgency_AlarmFlag_Set(unsigned char en);


__END_C_PROTO__
/**
 * @}
 */
#endif /* APPLIB_VIDEO_ECL_ADAS_H_ */

