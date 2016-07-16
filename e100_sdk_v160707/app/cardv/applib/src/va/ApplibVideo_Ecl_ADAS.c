/**
 * @file src/app/connected/applib/src/va/ApplibVideoAnal_LDFCWS.c
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

#include <gps.h>
#include <Ecl_ADAS.h>
#include <sys/time.h>
#include "../../../cardv/app/apps/gui/resource/gui_resource.h"
#include "../../../cardv/app/apps/gui/resource/gui_table.h"
AMBA_KAL_MUTEX_t ADAS_Proc_Mutex;
#define ADAS_PROC_LOC AmbaKAL_MutexTake(&ADAS_Proc_Mutex, AMBA_KAL_WAIT_FOREVER);
#define ADAS_PROC_UNL AmbaKAL_MutexGive(&ADAS_Proc_Mutex);


 enum
{
  UART_CMD_NONE = 0,
  UART_CMD_ADC,
  UART_CMD_FFT,
  UART_CMD_OBJECT,
  UART_CMD_ADJUST_OPEN,
  UART_CMD_ADJUST_CLOSE,
  UART_CMD_FLASH_UPDATE,
  UART_CMD_ENABLE_UART_DATA,
  UART_CMD_DISABLE_UART_DATA,
  UART_CMD_QUEST_USERDATA,
  // UART_CMD_ENABLE_ADJUST_PHASE,  // 10
  // UART_CMD_ADJUST_PHASE,
  UART_CMD_TEST, 
  UART_CMD_MAX,
} K_UART_CMD;

typedef struct 
{
    unsigned short flag;
    unsigned short size;
    unsigned short index;
    unsigned short cmd;
    float diff_ph;
    unsigned short temp1;
    unsigned short temp2;
}RADAR_CMD_CAL_CMD_T;
short acc_temp;

/*************************************************************************
 * GUI layout settings
 ************************************************************************/
#define ADAS_FUN(fmt, args...)      fprintf(stderr, fmt, ## args)
#define ADAS_ERR(fmt, args...)      fprintf(stderr, fmt, ## args)
// static gps_data_t* ApplibAdasGpsData = NULL;
ADAS_INFO           m_adas_info;
static long t_count=0;

#define ADAS_VIDEO_WIDTH (1280)
#define ADAS_VIDEO_HEIGHT (720)
#define DISPLAY_BUFFER_WIDTH (1024)
#define DISPLAY_BUFFER_HEIGHT (240)

static void AppLibVideo_Ecl_ADAS_timer_handler(int eid);
static unsigned long AppLibVideo_Ecl_ADAS_Get_Time_Count(void);
static void AppLibVideo_Ecl_FCWD_Event_Proc(ECL_ADAS_MSG_E msg, int distance_t);
static void AppLibVideo_Ecl_Lane_Event_Proc(ECL_ADAS_MSG_E msg, int l_none);

static unsigned char LdwsEventFlg=0;
static unsigned char FcwsEventFlg=0;
static unsigned char UcwsEventFlg=0;
static  unsigned long Event_Lane_Alarm_Timestamp=0;
static  unsigned long Event_Fcws_Alarm_Timestamp=0;

static unsigned char TTC_Emg=0;
static unsigned char TTC_Note=0;
static unsigned char UFCW_Start_Flg=2;
static unsigned char UFCW_Fcst_flg=0;
static int UFCW_Alarm_Dist=0;
static unsigned char Ufcw_mode_flg=0; //Enter LOw speed mode

static int Fcm_Alarm_Dist=0;
static long UFCW_Alarm_Timestamp=0;
static  unsigned long TTC_EMG_Alam=0;
static  unsigned long TTC_Note_Alam=0;
#define MAX_DANGEROURS_WIDTH (25.0f) //30
#define MAX_DETECTION_WIDTH  (32.0f) //30
#define _ABS(num)          ((num>0)?(num):-(num))

static char cmd_send[8];
static K_RADAR_OBJ_INFO * pRadar_ObjData=NULL;

void Send_ADAS_Msg_To_Alarm_Box(char *msg_send,int cmd_size)
{
    AmbaUART_Write(AMBA_UART_CHANNEL0, cmd_size,(UINT8 *)msg_send, AMBA_KAL_WAIT_FOREVER);
}


K_RECT_INT k_Rect_int(int x, int y, int w, int h)
{

  K_RECT_INT k_Rect_int = {x,y,w,h}; 
  return k_Rect_int;

}

//=====================================================================
static void Radar_Ufcw_Calc(K_RADAR_OBJ_INFO *pResult)
{
    int i;
  
    static int fps_count=0;
    static int flash_count=0;
    static int fcms_count=0;

    short obj_speed=0;
    int min_obj_distance=800;
    if(Ufcw_mode_flg==0)
    {
        Ufcw_mode_flg=1;
    }

    for(i=0;i<pResult->obj_num;i++)
    {
        K_RADAR_OBJ *pObj = &pResult->objs[i];
        // int v = pResult->abs_speed;
        float sin_th = (float)pObj->h/pObj->s;
        
        if(pResult->obj_num>8)
        {
           break;
        }

        if( (pObj->r > 12) && (_ABS(pObj->h) < (2-_ABS(sin_th))*20/4) )//MAX_DANGEROURS_WIDTH low speed 
        {
            if((pObj->s>20) && (pObj->s<min_obj_distance) && (pObj->p>70000/(pObj->s+100))  )
            {
                min_obj_distance=pObj->s;
                obj_speed=pObj->v;
            }
        }
    }

 //==============================================================================   
  if((pResult->acc_std<300)&&(pResult->abs_speed>-5 )&&(m_adas_info.fcmrs_mode_onoff==1))
  {
       if(UFCW_Fcst_flg==0&&min_obj_distance<60&&_ABS(obj_speed)<15)
       {
            UFCW_Fcst_flg=1;
            flash_count=0;
            Fcm_Alarm_Dist=min_obj_distance;  //Get stop distance
            fcms_count=0;
       }
       else if( UFCW_Fcst_flg==1&&(Fcm_Alarm_Dist>min_obj_distance)&&(min_obj_distance>20)&&_ABS(obj_speed)<15 )
       {
           Fcm_Alarm_Dist=min_obj_distance;//Get Nearer stop distance 
           fcms_count=0;
       }
       else if( UFCW_Fcst_flg==1&&( min_obj_distance>(30+Fcm_Alarm_Dist)&&(_ABS(pResult->abs_acc)<80)&&(pResult->acc_std<300)&&(obj_speed>40) ) )//Front car move>3m
       {
            UFCW_Fcst_flg=2;
            sprintf(cmd_send,"$%02d%d%d&",0,0,7); 
            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
            UFCW_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
            UcwsEventFlg=1;
       }
  }
  else if((_ABS(pResult->abs_speed)>50)||(pResult->acc_std>400))
  {
       if(fcms_count<5)
       {
          fcms_count++; 
       }
       else
       {
            UFCW_Fcst_flg=0;
       }    
  }       

//================================================================================
    if((min_obj_distance<=45)&&(m_adas_info.fcws_mode_onoff==1))
    {
        if(UFCW_Start_Flg==0)
        {
              UFCW_Start_Flg=1;
              UFCW_Alarm_Dist=min_obj_distance;
              if((UFCW_Alarm_Dist>20)&&(UFCW_Alarm_Dist<35)&&(AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_EMG_Alam+20))
              {
                  sprintf(cmd_send,"$%02d%d%d&",0,4,3); 
                  Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                  UFCW_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                  UcwsEventFlg=1;
              }
        }
        else if( (min_obj_distance>35&&UFCW_Alarm_Dist>=10+min_obj_distance )||(min_obj_distance<=35&&UFCW_Alarm_Dist>=5+min_obj_distance))
        {
             if((UFCW_Start_Flg==1)&&(AppLibVideo_Ecl_ADAS_Get_Time_Count()>=UFCW_Alarm_Timestamp+10)&&(pResult->acc_std>100))
             {
                  sprintf(cmd_send,"$%02d%d%d&",0,4,3); 
                  Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                  UFCW_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                  UcwsEventFlg=1;
             }  
             UFCW_Alarm_Dist=min_obj_distance;                                    
        }
        fps_count=0;
    }
    else if(min_obj_distance>50)
    {
        if(fps_count<60)
        {
            fps_count++;
        }
        else
        {
            fps_count=0;
            if(UFCW_Start_Flg==1)
            {
                UFCW_Start_Flg=0;
                min_obj_distance=80;
            }
        }   
    }

    if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>UFCW_Alarm_Timestamp+10)
    {
        static u8 loop_cs=0;
        if(loop_cs==0)
        {
            loop_cs++;
            sprintf(cmd_send,"$%02d%d%d&",0,2,0); 
            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
            FcwsEventFlg=1;
            Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();  
        }
        else if(loop_cs++>5)
        {
            loop_cs=0;
        }
    }
}
//================================================================================

static void Radar_Calc_Fcws_TTC(K_RADAR_OBJ_INFO *pResult)
{
     int i;
    static u8 Alarm_non = 0;
    static u8 ShowCarFlg=0;
    static u8 fcw_fps_count=0;

    static float obj_r=0.0f;

    static int obj_distance_alarm_flg=0;

    static int obj_show_filter=0;
    static int obj_dis_filter=0;
    static int old_status=0;

    int ttc_min=8000;
    int obj_distance=1000;
    short speed=0;
    int DetDistance=0;

    if(UFCW_Start_Flg==2)
    {
       UFCW_Start_Flg=0;
    }

    if(Ufcw_mode_flg==1)
    {
        UFCW_Start_Flg=0;
        UFCW_Fcst_flg=0;
        Ufcw_mode_flg=0; //Clear low mode
        sprintf(cmd_send,"$%02d%d%d&",0,0,8);
        Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
   } 

    //========================================Calculate the ttc_min && distance min===================================
    for(i=0;i<pResult->obj_num;i++)
    {
        K_RADAR_OBJ *pObj = &pResult->objs[i];
        int v = pResult->abs_speed;
        float sin_th = (float)pObj->h/pObj->s;

        if(pResult->obj_num>8)
        {
           break;
        }

        if( (pObj->r > 12) && (pObj->v > v*31/32+30) && (_ABS(pObj->h) < (2-_ABS(sin_th))*(MAX_DANGEROURS_WIDTH+obj_r)/4)&&(pResult->acc_std>200) )//obj in front
        {
            int ttc=Calc_fcws_ttc(v, pObj->v,pObj->s);
          
            if( (ttc>0) && (ttc<ttc_min ) && ( (pObj->s> 50) || (pObj->s>(-v/30+22) )) && ( pObj->p>70000/(pObj->s+100) ) )
            {
                ttc_min=ttc;
                if(obj_r<=3 && v<(-30) )
                {
                    obj_r+=0.03f;
                }  
            }
           
            if((pObj->v>=-400)&&(pObj->s>20)&&(pObj->s<=(-v/20+25) )&& ( pObj->p>70000/(pObj->s+100)) )
            {
                obj_distance_alarm_flg=1;  //near distance alarm
            }
        } 

        if( (pObj->r > 12) && (_ABS(pObj->h) < (2-_ABS(sin_th))*MAX_DETECTION_WIDTH/4) ) //+obj_r/4
        {
            if(pObj->s<obj_distance)
            {
                if( ( (pObj->s> 50) || (pObj->s>(-v/30+22) )) && (pObj->p>70000/(pObj->s+100)) )//
                {
                    obj_distance=pObj->s;
                    speed=pObj->v;
                } 
            }
        }
    }
   //====================Show OBJ_ICON less than 40m ===========================================  
      DetDistance=(-pResult->abs_speed)<=500?(-pResult->abs_speed):500;

      if(obj_distance<=DetDistance )//400))//( 50+ m_adas_info.speed*4) ))
      {
          if(pResult->abs_speed <=-300)//OBJ UP than 30km/h show ICON 
          {
               ShowCarFlg=1; 
          }
          else if( (pResult->abs_speed >-300) && (speed > (9*pResult->abs_speed/10+10)) && (speed < -pResult->abs_speed))//OBJ Less than 30 show
          {
              ShowCarFlg=1; 
          } 
          else
          {
              ShowCarFlg=0;

          }       
      }
      else if((ShowCarFlg==1) && (obj_distance>DetDistance+50) )//SHow ICON distance Filter 5 meters
      {
          ShowCarFlg=0;
          obj_r=-0.0f;   
      }
  //===============OBJ_FILTER==================================================== 

   if(old_status==0)
   {
      if(ttc_min<3100||ShowCarFlg==1)
      {
          if(obj_show_filter<10)
          {
              obj_show_filter++;
              return ;
          }
          old_status=1;
          obj_dis_filter=0;
      }
   }
   else if(old_status==1)
   {
      if(!(ttc_min<3100||ShowCarFlg==1))
      {
          if(obj_dis_filter<10)
          {
              obj_dis_filter++;
              return ;
          }
          old_status=0;
          obj_show_filter=0;
      }
   }

if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>(TTC_EMG_Alam+10)&&AppLibVideo_Ecl_ADAS_Get_Time_Count()>(TTC_Note_Alam+10))
{
      if((obj_distance_alarm_flg==1)&&(m_adas_info.fcws_mode_onoff==1))
      {
          sprintf(cmd_send,"$%02d%d%d&",0,4,3); //
          Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                          
          FcwsEventFlg=1;
          TTC_EMG_Alam=AppLibVideo_Ecl_ADAS_Get_Time_Count();
          Event_Fcws_Alarm_Timestamp=TTC_EMG_Alam;  
          return;
      }
        
}
obj_distance_alarm_flg=0;
//===========================Alarm flow=========================================
  switch(m_adas_info.adas_sensitivity_level) //LOW LEVEL 0.8S
  {
  case 0:   
        if( (ttc_min>=10) && (ttc_min<900)&&(m_adas_info.fcws_mode_onoff==1)) //r>30 ttc_min>10 &&
        {
            ttc_min = ttc_min/100; 
            fcw_fps_count=0;
            if(TTC_Emg==0)   //Emgency Alarm 
            {
                if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>(TTC_EMG_Alam+10))
                {
                    TTC_Emg=1;
                    TTC_Note=1;
                    if(pResult->abs_speed<=-300)
                      sprintf(cmd_send,"$%02d%d%d&",ttc_min,4,3); //Show distance when speed up 30km/h
                    else
                      sprintf(cmd_send,"$%02d%d%d&",0,4,3); //

                    Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                      
                    FcwsEventFlg=1;
                    TTC_EMG_Alam=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                    Event_Fcws_Alarm_Timestamp=TTC_EMG_Alam;    
                    Alarm_non=0;
                  } 
              }
              else if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_EMG_Alam+10)
              {
                   if(Alarm_non == 0)
                   {
                        Alarm_non=1;  
                        if(pResult->abs_speed<=-300) //Show distance when speed up 30km/h            
                        {
                            sprintf(cmd_send,"$%02d%d%d&",ttc_min,4,0); //  
                        } 
                        else
                        {
                            sprintf(cmd_send,"$%02d%d%d&",0,4,0); //  show ICON only
                        }
                        Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);      
                        FcwsEventFlg=1;
                        Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();     
                   }
                   else if(Alarm_non++>5)
                   {
                      Alarm_non=0;
                   }
              }
          }
          else if((ttc_min>=900)&& (ttc_min<1600)&&(m_adas_info.fcws_mode_onoff==1))
          {
              if(ttc_min>=1200 )
              {
                  TTC_Emg=0;   
              }

              fcw_fps_count=0;
              ttc_min = ttc_min/100;    
              if(TTC_Note==0)
              {
                  if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>(TTC_Note_Alam+10))
                  {
                     
                      TTC_Note=1;
                      if(pResult->abs_speed<=-300)  //up than 30km/h
                      {
                         sprintf(cmd_send,"$%02d%d%d&",ttc_min,4,2);
                      } 
                      else 
                      {
                         sprintf(cmd_send,"$%02d%d%d&",0,4,2);
                      }

                      Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                      FcwsEventFlg=1;
                      TTC_Note_Alam=AppLibVideo_Ecl_ADAS_Get_Time_Count();  //"zhu yi che ju" 
                      Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                      Alarm_non=0;
                  }
              }
              else if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_Note_Alam+10)
              {
                  if(Alarm_non == 0)
                  {
                      Alarm_non=1;
                      if(pResult->abs_speed<=-300) //show 0.9----1.5s  
                      {
                          sprintf(cmd_send,"$%02d%d%d&",ttc_min,4,0);
                      }
                      else
                      {
                          sprintf(cmd_send,"$%02d%d%d&",0,4,0);
                      }
                      Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                      FcwsEventFlg=1;
                      Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                  }
                  else if(Alarm_non++>5)
                  {
                      Alarm_non=0;
                  }
              }
          } 
          else if((ttc_min>=1600)&& (ttc_min<3100)&&(m_adas_info.hmws_mode_onoff==1) )//1.6-----------3.0 show Grean
          {

              if(ttc_min>=1800)
              {
                   if(fcw_fps_count<30)
                   {
                       fcw_fps_count++;
                   }
                   else
                   {
                       fcw_fps_count=0;
                       TTC_Note=0;
                   }        
              } 
              TTC_Emg=0;
              ttc_min = ttc_min/100;

              if(Alarm_non==0) 
              {
                  Alarm_non=1;
                  if(pResult->abs_speed<=-300)
                  {
                      sprintf(cmd_send,"$%02d%d%d&",ttc_min,0,0);
                  }
                  else
                  {
                      sprintf(cmd_send,"$%02d%d%d&",0,0,0);
                  }

                  if((AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_EMG_Alam+10)&&(AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_Note_Alam+10))
                  {
                      Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                      FcwsEventFlg=1;
                      Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count(); 
                  }
               }
               else if(Alarm_non++>5)
               {
                   Alarm_non=0;
               }       
                 
          }
          else if(ShowCarFlg==1&&(m_adas_info.hmws_mode_onoff==1))
          {
              if(Alarm_non==0) 
              {
                  Alarm_non=1;
                  if((AppLibVideo_Ecl_ADAS_Get_Time_Count()>=TTC_EMG_Alam+10)&&(AppLibVideo_Ecl_ADAS_Get_Time_Count()>=TTC_Note_Alam+10))
                  {
                      sprintf(cmd_send,"$%02d%d%d&",0,0,0);
                      Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                      FcwsEventFlg=1;
                      Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                  }
              }
              else if(Alarm_non++>5)
              {
                  Alarm_non=0;
              }    
              TTC_Note=0;
              TTC_Emg=0;
          }
          else if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>=(Event_Fcws_Alarm_Timestamp+10))
          {
              Alarm_non=0;
              TTC_Note=0;
              TTC_Emg=0;
          }
          break;
//==============================SEN LEVEL HIGH==================================================================================================
          case 2:
                if( (ttc_min>=10) && (ttc_min<1300)&&(m_adas_info.fcws_mode_onoff==1)) //r>30 ttc_min>10 &&
                {
                    ttc_min = ttc_min/100; 
                    fcw_fps_count=0;
                    if(TTC_Emg==0)   //Emgency Alarm 
                    {
                        if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>(TTC_EMG_Alam+10))
                        {
                            TTC_Emg=1;
                            TTC_Note=1;
                            if(pResult->abs_speed<=-300)
                              sprintf(cmd_send,"$%02d%d%d&",ttc_min,4,3); //Show distance when speed up 30km/h
                            else
                              sprintf(cmd_send,"$%02d%d%d&",0,4,3); //

                            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                            
                            FcwsEventFlg=1;
                            TTC_EMG_Alam=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                            Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();    
                            Alarm_non=0;
                        } 
                    }
                    else if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_EMG_Alam+10)
                    {
                       if(Alarm_non == 0)
                       {
                          Alarm_non=1;  
                          if(pResult->abs_speed<=-300) //Show distance when speed up 30km/h            
                          {
                              sprintf(cmd_send,"$%02d%d%d&",ttc_min,4,0); //  
                          } 
                          else
                          {
                              sprintf(cmd_send,"$%02d%d%d&",0,4,0); //  show ICON only
                          }
                          Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);      
                          FcwsEventFlg=1;
                          Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();     
                       }
                       else if(Alarm_non++>5)
                       {
                          Alarm_non=0;
                       }
                    }
                }
                else if((ttc_min>=1300)&& (ttc_min<2100)&&(m_adas_info.fcws_mode_onoff==1))
                {
                    if(ttc_min>=1600)
                    {
                        TTC_Emg=0;
                    }

                    fcw_fps_count=0;
                    ttc_min = ttc_min/100;    
                    if(TTC_Note==0)
                    {
                        if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>(TTC_Note_Alam+10))
                        {
                            TTC_Note=1;
                            if(pResult->abs_speed<=-300)  //up than 30km/h
                            {
                               sprintf(cmd_send,"$%02d%d%d&",ttc_min,4,2);
                            } 
                            else 
                            {
                               sprintf(cmd_send,"$%02d%d%d&",0,4,2);
                            }

                            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                            FcwsEventFlg=1;
                            TTC_Note_Alam=AppLibVideo_Ecl_ADAS_Get_Time_Count();  //"zhu yi che ju" 
                            Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                            Alarm_non=0;
                        }
                    }
                    else if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_Note_Alam+10)
                    {
                        if(Alarm_non == 0)
                        {
                            Alarm_non=1;
                            if(pResult->abs_speed<=-300) //show 0.9----1.5s  
                            {
                                sprintf(cmd_send,"$%02d%d%d&",ttc_min,4,0);
                            }
                            else
                            {
                                sprintf(cmd_send,"$%02d%d%d&",0,4,0);
                            }
                            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                            FcwsEventFlg=1;
                            Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                        }
                        else if(Alarm_non++>5)
                        {
                            Alarm_non=0;
                        }
                    }
                } 
                else if((ttc_min>=2100)&& (ttc_min<3100)&&(m_adas_info.hmws_mode_onoff==1) )//1.6-----------3.0 show Grean
                {

                    if(ttc_min>=2600)
                    {
                         if(fcw_fps_count<30)
                         {
                             fcw_fps_count++;
                         }
                         else
                         {
                             fcw_fps_count=0;
                             TTC_Note=0;
                         }        
                    }
                    
                    TTC_Emg=0;

                    ttc_min = ttc_min/100;

                    if(Alarm_non==0) 
                    {
                        Alarm_non=1;
                        if(pResult->abs_speed<=-300)
                        {
                            sprintf(cmd_send,"$%02d%d%d&",ttc_min,0,0);
                        }
                        else
                        {
                            sprintf(cmd_send,"$%02d%d%d&",0,0,0);
                        }

                        if((AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_EMG_Alam+10)&&(AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_Note_Alam+10))
                        {
                            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                            FcwsEventFlg=1;
                            Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count(); 
                        }
                     }
                     else if(Alarm_non++>5)
                     {
                         Alarm_non=0;
                     }       
                       
                }
                else if(ShowCarFlg==1&&(m_adas_info.hmws_mode_onoff==1))
                {
                    if(Alarm_non==0) 
                    {
                        Alarm_non=1;
                        if((AppLibVideo_Ecl_ADAS_Get_Time_Count()>=TTC_EMG_Alam+10)&&(AppLibVideo_Ecl_ADAS_Get_Time_Count()>=TTC_Note_Alam+10))
                        {
                            sprintf(cmd_send,"$%02d%d%d&",0,0,0);
                            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                            FcwsEventFlg=1;
                            Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                        }
                    }
                    else if(Alarm_non++>5)
                    {
                        Alarm_non=0;
                    }    
                    TTC_Note=0;
                    TTC_Emg=0;
                }
                else if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>=(Event_Fcws_Alarm_Timestamp+10))
                {
                    Alarm_non=0;
                    TTC_Note=0;
                    TTC_Emg=0;
                }
              break;

//=============================SEN LEVEL MIDDLE==============================================================================                
            case 1:
            default:
                 if( (ttc_min>=10) && (ttc_min<1100)&&(m_adas_info.fcws_mode_onoff==1)) //r>30 ttc_min>10 &&
                {
                    ttc_min = ttc_min/100; 
                    fcw_fps_count=0;
                    if(TTC_Emg==0)   //Emgency Alarm 
                    {
                        if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>(TTC_EMG_Alam+10))
                        {
                            TTC_Emg=1;
                            TTC_Note=1;
                            if(pResult->abs_speed<=-300)
                              sprintf(cmd_send,"$%02d%d%d&",ttc_min,4,3); //Show distance when speed up 30km/h
                            else
                              sprintf(cmd_send,"$%02d%d%d&",0,4,3); //

                            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                            
                            FcwsEventFlg=1;
                            TTC_EMG_Alam=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                            Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();    
                            Alarm_non=0;
                        } 
                    }
                    else if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_EMG_Alam+10)
                    {
                       if(Alarm_non == 0)
                       {
                          Alarm_non=1;  
                          if(pResult->abs_speed<=-300) //Show distance when speed up 30km/h            
                          {
                              sprintf(cmd_send,"$%02d%d%d&",ttc_min,4,0); //  
                          } 
                          else
                          {
                              sprintf(cmd_send,"$%02d%d%d&",0,4,0); //  show ICON only
                          }
                          Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);      
                          FcwsEventFlg=1;
                          Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();     
                       }
                       else if(Alarm_non++>5)
                       {
                          Alarm_non=0;
                       }
                    }
                }
                else if((ttc_min>=1100)&& (ttc_min<1900)&&(m_adas_info.fcws_mode_onoff==1))
                {
                    if(ttc_min>=1500)
                    {
                        TTC_Emg=0;
                    }
                    fcw_fps_count=0;
                    ttc_min = ttc_min/100;    
                    if(TTC_Note==0)
                    {
                        if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>(TTC_Note_Alam+10))
                        {
                            TTC_Note=1;
                            if(pResult->abs_speed<=-300)  //up than 30km/h
                            {
                               sprintf(cmd_send,"$%02d%d%d&",ttc_min,4,2);
                            } 
                            else 
                            {
                               sprintf(cmd_send,"$%02d%d%d&",0,4,2);
                            }

                            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                            FcwsEventFlg=1;
                            TTC_Note_Alam=AppLibVideo_Ecl_ADAS_Get_Time_Count();  //"zhu yi che ju" 
                            Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                            Alarm_non=0;
                        }
                    }
                    else if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_Note_Alam+10)
                    {
                        if(Alarm_non == 0)
                        {
                            Alarm_non=1;
                            if(pResult->abs_speed<=-300) //show 0.9----1.5s  
                            {
                                sprintf(cmd_send,"$%02d%d%d&",ttc_min,4,0);
                            }
                            else
                            {
                                sprintf(cmd_send,"$%02d%d%d&",0,4,0);
                            }
                            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                            FcwsEventFlg=1;
                            Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                        }
                        else if(Alarm_non++>5)
                        {
                            Alarm_non=0;
                        }
                    }
                } 
                else if((ttc_min>=1900)&& (ttc_min<3100)&&(m_adas_info.hmws_mode_onoff==1) )//1.6-----------3.0 show Grean
                {

                    if(ttc_min>=2500)
                    {
                         if(fcw_fps_count<30)
                         {
                             fcw_fps_count++;
                         }
                         else
                         {
                             fcw_fps_count=0;
                             TTC_Note=0;
                         }        
                    }
                    
                    TTC_Emg=0;

                    ttc_min = ttc_min/100;

                    if(Alarm_non==0) 
                    {
                        Alarm_non=1;
                        if(pResult->abs_speed<=-300)
                        {
                            sprintf(cmd_send,"$%02d%d%d&",ttc_min,0,0);
                        }
                        else
                        {
                            sprintf(cmd_send,"$%02d%d%d&",0,0,0);
                        }

                        if((AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_EMG_Alam+10)&&(AppLibVideo_Ecl_ADAS_Get_Time_Count()>TTC_Note_Alam+10))
                        {
                            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                            FcwsEventFlg=1;
                            Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count(); 
                        }
                     }
                     else if(Alarm_non++>5)
                     {
                         Alarm_non=0;
                     }       
                       
                }
                else if(ShowCarFlg==1&&(m_adas_info.hmws_mode_onoff==1))
                {
                    if(Alarm_non==0) 
                    {
                        Alarm_non=1;
                        if((AppLibVideo_Ecl_ADAS_Get_Time_Count()>=TTC_EMG_Alam+10)&&(AppLibVideo_Ecl_ADAS_Get_Time_Count()>=TTC_Note_Alam+10))
                        {
                            sprintf(cmd_send,"$%02d%d%d&",0,0,0);
                            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
                            FcwsEventFlg=1;
                            Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
                        }
                    }
                    else if(Alarm_non++>5)
                    {
                        Alarm_non=0;
                    }    
                    TTC_Note=0;
                    TTC_Emg=0;
                }
                else if(AppLibVideo_Ecl_ADAS_Get_Time_Count()>=(Event_Fcws_Alarm_Timestamp+10))
                {
                    Alarm_non=0;
                    TTC_Note=0;
                    TTC_Emg=0;
                }
              break;
        }
}
//====================User setting==========================================
static float g_radar_offset=0;
static float g_radar_offset_e = 1.0f;
static float g_radar_offset_old=-5;
static unsigned char Cal_En_Flg=0;

void AppLibVideo_Set_Adas_Ldws_OnOff(int ldws_mode)
{
    ADAS_PROC_LOC
    m_adas_info.ldws_mode_onoff=ldws_mode;
    ADAS_PROC_UNL
}

void AppLibVideo_Set_Adas_Fcws_OnOff(int fcws_mode)
{
    ADAS_PROC_LOC
    m_adas_info.fcws_mode_onoff=fcws_mode;
    ADAS_PROC_UNL
}

void AppLibVideo_Set_Adas_Hmws_OnOff(int hmws_mode)
{
    ADAS_PROC_LOC
    m_adas_info.hmws_mode_onoff=hmws_mode;
    ADAS_PROC_UNL
}

void AppLibVideo_Set_Adas_Fcmrs_OnOff(int fcmrs_mode)
{
    ADAS_PROC_LOC
    m_adas_info.fcmrs_mode_onoff=fcmrs_mode;
    ADAS_PROC_UNL
}

void AppLibVideo_Set_Adas_Sen_Level(int sen_level)
{
    ADAS_PROC_LOC
    m_adas_info.adas_sensitivity_level=sen_level;
    ADAS_PROC_UNL
}

void AppLibVideo_Set_Radar_Calibration_Mode(int mode_enable)
{
  ADAS_PROC_LOC
     if(mode_enable==1)
    {
        g_radar_offset_e=1;
        g_radar_offset=0;
        g_radar_offset_old=-5;
        Cal_En_Flg=1;
    }
    else
    {
        g_radar_offset_e=0;
        g_radar_offset_old=-5;
        Cal_En_Flg=0;
    }
    ADAS_PROC_UNL
}

void AppLibVideo_Set_Radar_Offset(float radar_offset)
{
    ADAS_PROC_LOC
    g_radar_offset=radar_offset;
    ADAS_PROC_UNL
}

float AppLibVideo_Get_Radar_Offset(void)
{
    return  g_radar_offset;
}

float AppLibVideo_Get_Radar_Offset_e(void)
{
    if(Cal_En_Flg==1)
    {
        return 1.0f;
    }
    else
    {
       return g_radar_offset_e;
    }
}
//======================================================================
short  acc_x=0;
short  acc_y=0;
short  acc_z=0;
short  acc_stb=0;
void AppLibVideo_Ecl_ADAS_Proc(UINT32 event, AMP_ENC_YUV_INFO_s* img)
{
    static K_RADAR_OBJ_INFO * pRadarObjData=NULL;

    // float diff_ph=0.0f;
    int i;

    pRadarObjData=AppLibSysGps_GetObjData();  //Get Radar Data
    pRadar_ObjData = pRadarObjData;
    m_adas_info.speed=-(pRadarObjData->abs_speed)/10;

    Ecl_ADAS_Set_Speed(m_adas_info.speed);//Update Current speed to video adas proc

    ECL_Video_ADAS_Proc((unsigned char *)img->yAddr);//Video Adas proc
    acc_x=pRadarObjData->acc_x;
    acc_y=pRadarObjData->acc_y;
    acc_z=pRadarObjData->acc_z;
    acc_stb=pRadarObjData->acc_std;
   //===============data Adjust============================================== 
    for(i=0;i<pRadarObjData->obj_num;i++)
    {
        float adjust_ph = 0.0f + g_radar_offset;
        pRadarObjData->objs[i].h += (short)(adjust_ph*pRadarObjData->objs[i].s/(2*GPS_PI));
    }

  //============Data Ajusting proc=======================================================
    if(g_radar_offset_e > 0.10f)
    {
        float diff_ph = 0;
        if(Ecl_ADAS_CombinWithRadar((ECL_RADAR_RESULT*)(pRadarObjData),&diff_ph)==0)
        {
            float diff = _ABS(diff_ph) - g_radar_offset_e;
            if(diff > 0)  
                g_radar_offset_e += 0.20f*diff;
            else      
                g_radar_offset_e += 0.02f*diff;

            g_radar_offset += 0.01f*diff_ph;
        }
        else if(g_radar_offset_e < 0.5f)
        {
            g_radar_offset_e += 0.0002f;
        }
    }

    if(g_radar_offset_e>0 && g_radar_offset_e<=0.1f &&Cal_En_Flg==1) 
    {
        if((0.05f<=_ABS(g_radar_offset_old-g_radar_offset))||(_ABS(g_radar_offset)>0.5f)) //offset>=0.05 drop old data
        {
            g_radar_offset_old=g_radar_offset;
            g_radar_offset_e=1.0f;
            g_radar_offset=0;
        }
        else
        {
            g_radar_offset=(g_radar_offset+g_radar_offset_old) /2; 
            Cal_En_Flg=0;//cal finished
        }          
    } 

 //=======================Combin with calibration offset======================================================= 
    if(m_adas_info.speed>=10)
        Radar_Calc_Fcws_TTC(pRadarObjData);
    else
        Radar_Ufcw_Calc(pRadarObjData);

}

void AppLibVideo_Ecl_ADAS_Init(int v_ratio)
{
    int i;
    k_MemSet(&m_adas_info, 0, sizeof(ADAS_INFO));
    ECL_Video_ADAS_Set_Paramentes(k_Rect_int(0,0,ADAS_VIDEO_WIDTH,ADAS_VIDEO_HEIGHT),v_ratio);
    m_adas_info.init=1; 
    // Ecl_Adas_Set_Proc_Mode(ADAS_PROC_NO_SPEED_LIMIT);
    Ecl_Adas_Set_Proc_Mode(ADAS_PROC_GPS_BASED);
}

static void Ecl_ADAS_Draw_Event_Proc(UINT32 event, AMP_ENC_YUV_INFO_s* img)
{
   
    int j;
    char text[64];
    int out_w = DISPLAY_BUFFER_WIDTH;
    int out_h = DISPLAY_BUFFER_HEIGHT;
    // unsigned int loop_c=0;
    K_RECT_INT krect_obj;
    ADAS_LDW_DETECT_OUTPUT_s sLdwOutInfo;
    K_POINT_INT LanePoint_Draw[2];
   
    ECL_Adas_GetLdwOutput(&sLdwOutInfo);

    if(Ecl_dc_draw_get_init_state()==0)
    {
        Ecl_dc_draw_init();
    }
    Ecl_draw_dc_create(out_w,out_h,img->yAddr);
    #if 1
    if(m_adas_info.ldws_mode_onoff)
    {

      if(sLdwOutInfo.LanNum0.IsDetected)
      {
        if(sLdwOutInfo.LanNum0.LwdWarringFlg==1)//||sLdwOutInfo.LanNum0.IsDetected==2)
        {
             Ecl_DrawUsePen(4, _RGB(255,0,0));
        }
        else
        {
            Ecl_DrawUsePen( 4, _RGB(0,255,0));
        }

        LanePoint_Draw[0].x=(int)(sLdwOutInfo.LanNum0.Points[0].x*960);
        LanePoint_Draw[0].y=(int)(sLdwOutInfo.LanNum0.Points[0].y*240);

        for(j=1;j<sLdwOutInfo.LanNum0.PointsCount;j++)
        {
            LanePoint_Draw[1].x=(int)(sLdwOutInfo.LanNum0.Points[j].x*960);
            LanePoint_Draw[1].y=(int)(sLdwOutInfo.LanNum0.Points[j].y*240);
            Ecl_DrawLine(LanePoint_Draw, 2);
             LanePoint_Draw[0].x=LanePoint_Draw[1].x;
             LanePoint_Draw[0].y=LanePoint_Draw[1].y;
        }
      }
  
      if(sLdwOutInfo.LanNum1.IsDetected)
      {
        if(sLdwOutInfo.LanNum1.LwdWarringFlg)
        {
            Ecl_DrawUsePen(4, _RGB(255,0,0));
        }
        else
        {
           Ecl_DrawUsePen(4, _RGB(0,255,0));
        }
        LanePoint_Draw[0].x=(int)(sLdwOutInfo.LanNum1.Points[0].x*960);
        LanePoint_Draw[0].y=(int)(sLdwOutInfo.LanNum1.Points[0].y*240);

        for(j=1;j<sLdwOutInfo.LanNum1.PointsCount;j++)
        {
          LanePoint_Draw[1].x=(int)(sLdwOutInfo.LanNum1.Points[j].x*960);
          LanePoint_Draw[1].y=(int)(sLdwOutInfo.LanNum1.Points[j].y*240);
          Ecl_DrawLine(LanePoint_Draw, 2);
          LanePoint_Draw[0].x=LanePoint_Draw[1].x;
          LanePoint_Draw[0].y=LanePoint_Draw[1].y;
        }
      }
}
  if(m_adas_info.fcws_mode_onoff)
  {
      ADAS_FCW_INFO_UNI_s sFcwOutInfo;
      ECL_Adas_GetFcwOutput(&sFcwOutInfo);
      // AmbaPrint("ECL ADAS ENABLE 22200\n");
      if(sFcwOutInfo.FcwActive==1)
      {
          for(j=0;j<ECL_FCW_OBJ_NUM;j++)
          {   
              if(sFcwOutInfo.ObjectVal[j]==1)
              {
                 if(sFcwOutInfo.ObjectWarring[j]==1)
                 // if(sFcwOutInfo.ObjectNeedCare[j]==1)
                 {
                    Ecl_DrawUsePen(4, _RGB(255,0,0));
                 }
                 else
                 {
                    Ecl_DrawUsePen(4, _RGB(0,255,0));
                 }
                  krect_obj.x=(int)(sFcwOutInfo.ObjectRect[j].x*960);
                  krect_obj.y=(int)(sFcwOutInfo.ObjectRect[j].y*DISPLAY_BUFFER_HEIGHT);
                  krect_obj.w=(int)(sFcwOutInfo.ObjectRect[j].w*960);
                  krect_obj.h=(int)(sFcwOutInfo.ObjectRect[j].h*DISPLAY_BUFFER_HEIGHT);
                  Ecl_DrawRectangle(krect_obj, 0);          
                  // sprintf(text, "%.1fm", (float)(sFcwOutInfo.ObjectDistance[j]/100));
                  // Ecl_DrawText(krect_obj.x,krect_obj.y-32, text,_RGB(0,255,255));
              }  
          }
    }
  }  
  #endif
  sprintf(text, "[%.2f] [%.2f]  [%02d]",g_radar_offset,g_radar_offset_e,acc_stb);// m_adas_info.speed);
  // sprintf(text, "[%03d] [%03d]  [%03d]",acc_x,acc_y, acc_z);
  Ecl_DrawText(320,120, text,_RGB(0,255,255));
}

void  AppLibVideo_ECL_ADAS_Set_Fcwd_ttc(unsigned int ttc_t)
{
      Ecl_ADAS_Set_TTC_Limit_t(ttc_t);
}

static ECL_ADAS_MSG_E fcwd_old_msg=MSG_ADAS_NULL;
static  unsigned long Event_Alarm_Timestamp[5][2]={{0,0},{0,0},{0,0},{0,0},{0,0}};
static void AppLibVideo_Ecl_FCWD_Event_Proc(ECL_ADAS_MSG_E msg, int distance_t)
{
    unsigned long t=0;
    static u8 value =0;
  

    t=AppLibVideo_Ecl_ADAS_Get_Time_Count();
    
    if(msg!=fcwd_old_msg)
    {
       fcwd_old_msg= msg;  
    }
    else if(msg==fcwd_old_msg)
    {
        if(t-Event_Alarm_Timestamp[msg][0]<40) //Same Event 
        {
            value++;
  	        if(value > 5)		
  	      	{
  	      	  value = 0;
  		        distance_t = distance_t/100;	  
              sprintf(cmd_send,"$%02d%d%d&",distance_t,4,0);
              Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
  	          FcwsEventFlg=1;
  		        Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
  	      	 }	   
             return ; 
        }
    }

    if(t-Event_Alarm_Timestamp[msg][1]<40) 
    {
        Event_Alarm_Timestamp[msg][0]=t;    
        return;
    }
    Event_Alarm_Timestamp[msg][1]=t;

    switch(msg)
    {
       case MSG_OBJ_FRONT_LEFT:
       case MSG_OBJ_FRONT_MIDDLE:
       case MSG_OBJ_FRONT_RIGHT:
	     //event_t = 4;	
            if(m_adas_info.fcws_mode_onoff==1)
            {
              distance_t = distance_t/100;
		          FcwsEventFlg=1;
              sprintf(cmd_send,"$%02d%d%d&",distance_t,4,1);
              Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
		          Event_Fcws_Alarm_Timestamp=AppLibVideo_Ecl_ADAS_Get_Time_Count();
            }
            break;

        case MSG_LAN_LEFT:
        case MSG_LAN_RIGHT:
        default:
             break;
    }
}


static void AppLibVideo_Ecl_Lane_Event_Proc(ECL_ADAS_MSG_E msg, int l_none)
{
  
     unsigned long t=0;
    
    t=AppLibVideo_Ecl_ADAS_Get_Time_Count();   
      
   if(t>=Event_Lane_Alarm_Timestamp+20)
    {
        if(m_adas_info.ldws_mode_onoff==1)
        {
           if(msg == MSG_LAN_LEFT)
	         {
              if(m_adas_info.speed>=60)
			         {
                    sprintf(cmd_send,"$%02d%d%d&",0,8,4); 
                }
                else
                {
                   sprintf(cmd_send,"$%02d%d%d&",0,8,0); 
                }           
			         LdwsEventFlg=1;
           }
          else if(msg == MSG_LAN_RIGHT)
		      {
			       // AmbaPrintAdas("$%02d%d%d&",0,9,2); 
             if(m_adas_info.speed>=60)
             {
                   sprintf(cmd_send,"$%02d%d%d&",0,9,4);
             }
             else
             {
                  sprintf(cmd_send,"$%02d%d%d&",0,9,0);
             }
                         
			       LdwsEventFlg=1;
          }
	   
        } 
      
        if(LdwsEventFlg==1)
        {
            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6); 
            Event_Lane_Alarm_Timestamp=t;
        }
    }

}

void AppLibVideo_Emgency_AlarmFlag_Set(unsigned char en)
{
    TTC_Emg = en;
}


K_RADAR_OBJ_INFO* AppLibVideo_Ecl_ADAS_GetObjData(void)
{
    return  pRadar_ObjData; 

}

int AppLibVideo_Ecl_ADAS_Enable(void)
{
     int ReturnValue = 0;
    if (!(m_adas_info.init)) {
        AppLibVideo_Ecl_ADAS_Init(95);
    }
    ReturnValue = AppLibVideoAnal_FrmHdlr_Register(0, AppLibVideo_Ecl_ADAS_Proc);
    //ReturnValue |= AppLibVideoAnal_FrmHdlr_Register(1, Ecl_ADAS_Draw_Event_Proc);
    Ecl_ADAS_FCWD_SetEventCallback(AppLibVideo_Ecl_FCWD_Event_Proc);
    Ecl_ADAS_Lane_SetEventCallback(AppLibVideo_Ecl_Lane_Event_Proc);
    AppLibComSvcTimer_Register(TIMER_10HZ, AppLibVideo_Ecl_ADAS_timer_handler);
    return ReturnValue;
}

void AppLibVideo_Ecl_ADAS_Disable(void)
{
    m_adas_info.init=0;
    m_adas_info.fcws_mode_onoff=0;
    m_adas_info.ldws_mode_onoff=0;
    AppLibVideoAnal_FrmHdlr_UnRegister(0, AppLibVideo_Ecl_ADAS_Proc);
   // AppLibVideoAnal_FrmHdlr_UnRegister(1, Ecl_ADAS_Draw_Event_Proc);
    AppLibComSvcTimer_Unregister(TIMER_10HZ, AppLibVideo_Ecl_ADAS_timer_handler);//TIMER_10HZ
    Ecl_ADAS_FCWD_SetEventCallback(NULL);
    Ecl_ADAS_Lane_SetEventCallback(NULL);
}

/**
 *  @brief The timer handler that shows gui for ADAS.
 *
 *  @param[in] eid timer id.
 *
 */
static void AppLibVideo_Ecl_ADAS_timer_handler(int eid)
{
    
    t_count++;
	 if(LdwsEventFlg==1)
	 {
  	 	if(t_count-Event_Lane_Alarm_Timestamp>=12)
  	 	{
    	 		  LdwsEventFlg=0;
            sprintf(cmd_send,"$%02d%d%d&",0,0,8);
            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
            Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
  	 	}		
	 }
	 else if( FcwsEventFlg==1)
	 {
  		if(t_count-Event_Fcws_Alarm_Timestamp>=12)
  		{
    			 FcwsEventFlg=0;
           sprintf(cmd_send,"$%02d%d%d&",0,0,8);
           Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
           Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
  		}
	}
  else if(UcwsEventFlg==1&&t_count-UFCW_Alarm_Timestamp>=12)
  {
      UcwsEventFlg=0;
      sprintf(cmd_send,"$%02d%d%d&",0,0,8);
      Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
      Send_ADAS_Msg_To_Alarm_Box(cmd_send,6);
  }
	 
}

static unsigned long AppLibVideo_Ecl_ADAS_Get_Time_Count(void)
{
    return t_count;
}

