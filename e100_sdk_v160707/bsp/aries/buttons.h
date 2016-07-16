/**
  * @file bsp/aries/buttons.h
  *
  * The header file of button setting
  *
  * History:
  *    2015.04.21 - [Yuchi Wei] created file
  *
  * Copyright (C) 2015, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef BSP_BUTTONS_H_
#define BSP_BUTTONS_H_

typedef enum _BSP_BUTTON_EVENT_e_
{
    UI_BUTTON_DOWN_EVENT,/* Button down */
    UI_BUTTON_UP_EVENT,/* Button up */
    UI_BUTTON_SET_EVENT/* Button set */
} BSP_BUTTON_EVENT_e;

typedef enum _BSP_BUTTON_ID_e_
{
    UP_BUTTON,/* 0 */
    DOWN_BUTTON,
    LEFT_BUTTON,
    RIGHT_BUTTON,
    VOLUME_UP_BUTTON,
    VOLUME_DOWN_BUTTON,
    FFW_BUTTON,
    REW_BUTTON,
    ZOOM_IN_BUTTON,
    ZOOM_OUT_BUTTON,
    POWER_BUTTON,/* 10*/
    RESET_BUTTON,
    RECORD_BUTTON,
    SNAP1_BUTTON,
    SNAP2_BUTTON,
    MODE_BUTTON,
    MENU_BUTTON,
    SET_BUTTON,
    PANEL_CLOSED_BUTTON,
    PANEL_ROTATED_BUTTON,
    PLAY_BUTTON,/* 20 */
    STOP_BUTTON,
    BLC_BUTTON,
    DISPLAY_BUTTON,
    DEL_BUTTON,
    MIRROR_BUTTON,
    BATT_COVER_DET_BUTTON,
    LCD_ON_BUTTON,
    LCD_OFF_BUTTON,
    VIO_BUTTON,
    F0_BUTTON,/* 30 */
    F1_BUTTON,
    F2_BUTTON,
    F3_BUTTON,
    F4_BUTTON,
    F5_BUTTON,
    F6_BUTTON,
    F7_BUTTON,
    F8_BUTTON,
    F9_BUTTON,

    IR_ZOOM_IN_BUTTON,/* 40 */
    IR_ZOOM_OUT_BUTTON,
    IR_REG_DOWN_BUTTON,
    IR_REG_UP_BUTTON,
    IR_UP_BUTTON,
    IR_DOWN_BUTTON,
    IR_LEFT_BUTTON,
    IR_RIGHT_BUTTON,
    IR_SET_BUTTON,
    IR_HUNDRED_BUTTON,
    IR_AUDIO_BUTTON,/* 50 */
    IR_MODE_BUTTON,
    IR_MENU_BUTTON,
    IR_BASS_LEFT_BUTTON,
    IR_RETURN_BUTTON,
    IR_IMAGE_BUTTON,
    IR_FOUR_THREE_BUTTON,
    IR_STANDARD_BUTTON,
    IR_INDEX_BUTTON,
    IR_MTS_BUTTON,
    IR_C1_BUTTON,/* 60 */
    IR_MUTE_BUTTON,
    IR_DISPLAY_BUTTON,
    IR_DEL_BUTTON,
    IR_N0_BUTTON,
    IR_SNAP1_BUTTON,
    IR_SNAP2_BUTTON,
    IR_RECORD_BUTTON,
    IR_N4_BUTTON,
    IR_N5_BUTTON,
    IR_N6_BUTTON,/* 70 */
    IR_N7_BUTTON,
    IR_N8_BUTTON,
    IR_N9_BUTTON
} BSP_BUTTON_ID_e;

#define UI_FIRST_BUTTON                 UP_BUTTON
#define UI_LAST_BUTTON                  IR_N9_BUTTON
#define UI_LAST_LVL_TRI_BUTTON          IR_N9_BUTTON

//#define GPIO_KEY_SET  GPIO_PIN_11
//#define GPIO_KEY_MENU  GPIO_PIN_12
//#define GPIO_KEY_SNAP2  GPIO_PIN_13
#define GPIO_KEY_POWER  GPIO_PIN_8

extern void A9RefButton_Init(void);
extern void BspRefBotton_jamed_handler_en(int enable);
#endif
