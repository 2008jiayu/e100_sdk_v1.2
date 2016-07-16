/**
 * @file NetBleCfg.h
 *
 * Copyright (C) 2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef __AMP_NETBLECFG_H__
#define __AMP_NETBLECFG_H__

#include <net/NetCtrl.h>
#include <../bt/amba_bluetooth.h>

/**
 * The BLE infomation of NetCtrlCfg
 */
typedef struct _AMP_NETBLECFG_BLEINFO_s_ {
    UINT32 BleWriteCharId;       /**< This chararictic is used to let the phone-App to write the data to Camera */
    UINT32 BleReadCharId;        /**< This chararictic is used to let the phone-App to read the data from Camera */
    UINT32 MsgBufSize;           /**< The size of message buffer (byte) */
    amba_bt_if_t *BleInterfaceIns;      /**< the pointer which is indicated the address of BLE interface instance
                                        (the instance of the structure "amba_bt_if_t")*/
    gatt_service_t *BleGattServiceIns;  /**< the pointer which is indicated the address of gatt service instance
                                        (the instance of the structure "gatt_service_t")*/
} AMP_NETBLECFG_BLEINFO_s;


/**
 * @brief  Register this function to the Gatt write service if you want to use the module "BleNetworkHandler" to
 *         handle the input when the Ble gets the writing requirement.
 *         Please Notice that this function just to support the attrId is same as the char_attr_id or desc_attr_id
 *         of chararictic which id is same as BleWriteCharId .
 * @return NULL
 */
extern void AmpNetBleCfg_GattWriteHandler(UINT16 attrId, const void *data, UINT16 len);

/**
 * @brief  This function is used to get the  Ble information in the module "BleNetworkHandler".
 * @param [in] The pointer is used to indicate the memory which is saved the returned ble information instance
 *             (the instance of the structure "AMP_NETBLECFG_BLEINFO_s")
 *
 * @return 0 - successfully
 * @return negative value - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetBleCfg_GetBleInfo(AMP_NETBLECFG_BLEINFO_s *bleInfoIns);

/**
 * @brief  This function is used to set the Ble informaton to the module
 *         "Ble_Network_Handler" after initialized.
 *         The error code will be returned if the module "BleNetworkHandler"
 *         has been initialized so that you should call the release fx. in the module "NetCtrl"
 *         , set the BLE info, and then call the init fx. in the module "NetCtrl" again.
 *
 * @param [in] The pointer is used to indicate the address of the ble information instance
 *             (the instance of the structure "AMP_NETBLECFG_BLE_INFO_s")
 *             Notice that it just let one set of Ble information
 *             existed at the same time.
 *
 * @return 0 - all Ble informaion has been set successfully
 * @return negative value - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetBleCfg_SetBleInfo(AMP_NETBLECFG_BLEINFO_s *bleInfoIns);

/**
 * @brief  This function is used to reset the resource but not to clean "BLE information"
 *         in the module "Ble_Network_Handler".
 *         Notice that the message buffer just is formatted but not deleted
 *         and the task "Ble_Msg_Daemon" still keep alive
 *
 * @return 0 - OK
 * @return negative value - AMP_NETCTRL_ERROR_e
 */
extern int AmpNetBleCfg_Reset(void);

/**
 * @brief  This function is used to clean the message buffer in the module
 *         "Ble_Network_Handler".
 *         Notice that the message buffer just is formatted but not deleted.
 *
 * @return 0 - OK
 * @return negative value - AMP_NETCTRL_ERROR_e
 */
//extern int AmpNetBleCfg_ResetMsgBuf(void);


#endif
