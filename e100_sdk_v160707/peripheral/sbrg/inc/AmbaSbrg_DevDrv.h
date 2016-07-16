#ifndef AMBA_SBRIDGE_DEV_DRV_H
#define AMBA_SBRIDGE_DEV_DRV_H

#include "AmbaVIN.h"
#include "AmbaSbrg_Ds90ub91x.h"

#define SBRG_CHAN_ID_S2		0x1
#define SBRG_CHAN_ID_S3D	0x2
#define SBRG_CHAN_ID_DS90UB91X	0x3

/* Data structure containing communication parameters.
 */
typedef struct _AMBA_SBRG_DEV_ACCESS_s_ {
	UINT16  channel;
#define SBRG_CH_UNUSED		0
#define SBRG_CH_LEFT		1
#define SBRG_CH_RIGHT		2
#define SBRG_CH_BOTH		3

	UINT16  protocol;
#define SBRG_PROTOCOL_IDC	0
#define SBRG_PROTOCOL_SPI	1

	UINT16  trans_mode;	/* Transmittion mode */
#define SBRG_DATA_NORMAL	0
#define SBRG_DATA_BURST		1

	UINT16  slave_addr;	/* IDC slave address */
#define SBRG_DEV_SELF		0xFFFFFFFF

	UINT16  slave_type;	/* IDC slave type */

	UINT16  reg_addr;		/* Data location of slave device to */
				/* be accessed */
	UINT16  reg_data[16];	/* Data buffer for transmission */
	UINT16  reg_count;		/* Number of data to be accessed */

	UINT16  master_inst;	/* SPI master instance */
	UINT16  slave_select;	/* SPI slave device connected to */
				/* a specified master */
} AMBA_SBRG_DEV_ACCESS_s;

__BEGIN_C_PROTO__

/**
 * Configure Main function for SBridge
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @param sub_module_id - the sub-module id.
 * @param cfg 		- the pointer to config for the specified sub-module.
 * @return - 0 succeed, < 0 failed.
 */
extern int AmbaSbrg_Config(UINT8 sbrg_chan_id, UINT16 sub_module_id, void *cfg);

/**
 * Reset function for SBridge
 *
 * @param mode - the point to the reset mode.
 * @return - 0 successful, < 0 failed
 */
extern int AmbaSbrg_Reset(UINT8 sbrg_chan_id, UINT16 mode);

/**
 * Write the registers
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @param addr	 - the register address to set
 * @param data - the register data to set
 * @param size - the data size to set
 * @return - 0 successful, < 0 failed
 */
extern int AmbaSbrg_WriteReg(UINT8 sbrg_chan_id, UINT16 addr, UINT16 data, UINT16 size);

/**
 * Write the registers
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @param addr - the register address to set
 * @param data - the register data to set
 * @param size - the data size to set
 * @return - 0 successful, < 0 failed
 */
extern int AmbaSbrg_BurstWriteReg(UINT8 sbrg_chan_id, UINT16 addr, UINT16 data, UINT16 size);

/**
 * Read the register value
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @param addr - the register address to set
 * @param data - the register data to set
 * @param size - the data size to set
 * @return - 0 successful, < 0 failed
 */
extern int AmbaSbrg_ReadReg(UINT8 sbrg_chan_id, UINT16 addr, UINT8 *data, UINT16 size);

/**
 * Write the registers to specified device
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @param tx_param - the device and register information
 * @return - 0 successful, < 0 failed
 */
extern int AmbaSbrg_WriteDevice(UINT8 sbrg_chan_id, AMBA_SBRG_DEV_ACCESS_s *tx_param);

/**
 * Read the register value from specified device
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @param rx_param - the device and register information
 * @return - 0 successful, < 0 failed
 */
extern int AmbaSbrg_ReadDevice(UINT8 sbrg_chan_id, AMBA_SBRG_DEV_ACCESS_s *rx_param);

/**
 * Dump the information for SBridge
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @return - 0 successful, < 0 failed
 */
extern int AmbaSbrg_GetInfo(UINT8 sbrg_chan_id);

/**
 * Get SBridge device
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @retrun - 0 succussful, < 0 failed
 */
extern int AmbaSbrg_GetDeviceInfo(UINT8 sbrg_chan_id, char *sbrg_name);

/**
 * Enable SBridge device
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @return - 0 successful, < 0 failed
 */
extern int AmbaSbrg_Enable(UINT8 sbrg_chan_id);

/**
 * Disable SBridge device
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @return - 0 successful, < 0 failed
 */
extern int AmbaSbrg_Disable(UINT8 sbrg_chan_id);

/**
 * Make SBridge caputre only one frame
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @param func - the callback function to stop streaming of video input source.
 * @return - 0 successful, < 0 failed
 */
extern int AmbaSbrg_CaptureOneFrame(UINT8 sbrg_chan_id, void *func);

#ifdef AMBA_SBRG_DEV_IMPL


/* Data structure containing service functions that need to be provided to the
 * Sbridge device manager by the device drivers.
 */
typedef struct _AMBA_SBRG_DEV_s_ {
	char name[30];
	int  (*config) (void *arg1, void *arg2);
	int  (*reset) (void *arg);
	int  (*enable) (void);
	int  (*disable) (void);
	int  (*write_reg) (UINT16 addr, UINT16 data, UINT16 size);
	int  (*burst_write_reg) (UINT16 addr, UINT16 data, UINT16 size);
	int  (*read_reg) (UINT16 addr, UINT8 *data, UINT16 size);
 	int  (*write_dev) (AMBA_SBRG_DEV_ACCESS_s *param);
	int  (*read_dev) (AMBA_SBRG_DEV_ACCESS_s *param);
	int  (*info) (struct _AMBA_SBRG_DEV_s_ *dev);
	int  (*capture_one_frm) (void *func);

} AMBA_SBRG_DEV_s;

/**
 * Attach the SBridge device driver and enable the device control.
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @param sbridge - the data structure containing service functions
 * @return - 0 successful, < 0 failed
 * @see also sbridge_dev_t
 */
extern int AmbaSbrg_DriverAttach(UINT8 sbrg_chan_id, AMBA_SBRG_DEV_s *sbrg);

/**
 * Fully remove the SBridge device driver
 *
 * @param sbrg_chan_id	- sbrg device chan id
 * @return - 0 successful, < 0 failed
 */
extern int AmbaSbrg_DriverRemove(UINT8 sbrg_chan_id);

#endif

__END_C_PROTO__

#endif /* AMBA_SBRIDGE_DEV_DRV_H */
