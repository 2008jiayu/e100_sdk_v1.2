#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AmbaDataType.h"
#include "AmbaKAL.h"
#include "AmbaI2C.h"
#include "AmbaPrintk.h"

#define AMBA_SBRG_DEV_IMPL
#include "AmbaSbrg_DevDrv.h"


#define IDC_ADR_DS90UB914Q_W	0xc0
#define IDC_ADR_DS90UB914Q_R	0xc1

#ifdef CONFIG_BSP_LIBRA
#define IDC_ADR_DS90UB913Q_W	0xb2
#define IDC_ADR_DS90UB913Q_R	0xb3
#else
#define IDC_ADR_DS90UB913Q_W	0xb0
#define IDC_ADR_DS90UB913Q_R	0xb1
#endif

#define DSQ_DEBUG
#ifdef DSQ_DEBUG
#define DEBUG_MSG	AmbaPrint
#else
#define DEBBUG_MSG(...)
#endif

typedef struct ds90ub91x_obj_s {
	UINT16	slave_id_w;
	UINT16	slave_id_r;
} ds90ub91x_obj_t;

//static __ARMCC_ALIGN(4) ds90ub91x_obj_t G_ds90ub91x __GNU_ALIGN(4);
static ds90ub91x_obj_t G_ds90ub91x;


static void Ds90ub91x_RegWR (UINT16 Addr, UINT8 Data, UINT16 size, UINT8 burst)
{
    UINT8 TxDataBuf[2];

    TxDataBuf[0] = (UINT8) (Addr & 0xff);
    TxDataBuf[1] = Data;

    if (AmbaI2C_Write(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                      (UINT8)G_ds90ub91x.slave_id_w, 2, TxDataBuf,
                      AMBA_KAL_WAIT_FOREVER) == NG)
        AmbaPrint("I2C does not work!!!!!");
}

static void Ds90ub91x_RegRD (UINT16 Addr, UINT8 *Data, UINT16 size)
{
    UINT16 pTxData[3];

    pTxData[0] = (AMBA_I2C_RESTART_FLAG | (G_ds90ub91x.slave_id_w));
    pTxData[1] = (Addr & 0xff);  /* Register Address [7:0]  */
    pTxData[2] = (AMBA_I2C_RESTART_FLAG | (G_ds90ub91x.slave_id_r));

    AmbaI2C_ReadAfterWrite(AMBA_I2C_CHANNEL0, AMBA_I2C_SPEED_STANDARD,
                                  3, (UINT16 *) pTxData, 1, Data, AMBA_KAL_WAIT_FOREVER);
}

/**
 * Initializatoin function for DS90UB914Q
 */
static int Ds90ub91x_Init914q(UINT16 device_slave_id)
{
	UINT8 rval = 0;
	UINT8 reg;

	G_ds90ub91x.slave_id_w = IDC_ADR_DS90UB914Q_W;
	G_ds90ub91x.slave_id_r = IDC_ADR_DS90UB914Q_R;

	DEBUG_MSG("init ds90ub914q ...");

	Ds90ub91x_RegRD(0x00, &rval, 1); /* I2C Device ID */
	if (rval == 0xff || rval == 0x00) {
		DEBUG_MSG("unable to access ds90ub914q!");
		return -1;
	}

	reg = 0x01; //Reset
	Ds90ub91x_RegWR(0x01, reg, 1, 0);
	DEBUG_MSG("Write r0x01 = 0x%x", reg);

	AmbaKAL_TaskSleep(1);

	reg = 0xe9;
	Ds90ub91x_RegWR(0x03, reg, 1, 0);
	DEBUG_MSG("Write r0x03 = 0x%x", reg);

#ifdef CONFIG_BSP_LIBRA
	reg = 0xb2;
#else
	reg = 0xb0;
#endif	
	Ds90ub91x_RegWR(0x06, reg, 1, 0);
	DEBUG_MSG("Write r0x06 = 0x%x", reg);
	Ds90ub91x_RegWR(0x07, reg, 1, 0);
	DEBUG_MSG("Write r0x07 = 0x%x", reg);

	reg = device_slave_id;
	Ds90ub91x_RegWR(0x08, reg, 1, 0);
	DEBUG_MSG("Write r0x08 = 0x%x", reg);
	Ds90ub91x_RegWR(0x10, reg, 1, 0);
	DEBUG_MSG("Write r0x10 = 0x%x", reg);

	DEBUG_MSG("init ds90ub914q done!");

	return 0;
}

/**
 * Initializatoin function for DS90UB913Q
 */

static int Ds90ub91x_Init913q(void)
{
	UINT8 rval = 0;
	UINT16 reg;

	G_ds90ub91x.slave_id_w = IDC_ADR_DS90UB913Q_W;
	G_ds90ub91x.slave_id_r = IDC_ADR_DS90UB913Q_R;

	DEBUG_MSG("init ds90ub913q ...");

	Ds90ub91x_RegRD(0x00, &rval, 1); /* I2C Device ID */
	if (rval == 0xff || rval == 0x00) {
		DEBUG_MSG("unable to access ds90ub913q!");
		return -1;
	}

	reg = 0x21; //Reset
	Ds90ub91x_RegWR(0x01, reg, 1, 0);
	DEBUG_MSG("Write r0x01 = 0x%x", reg);

	AmbaKAL_TaskSleep(1);

	reg = 0x20; //power 1.8v
	Ds90ub91x_RegWR(0x01, reg, 1, 0);
	DEBUG_MSG("Write r0x01 = 0x%x", reg);

	reg = 0x18; //I2C 400k
	Ds90ub91x_RegWR(0x11, reg, 1, 0);
	DEBUG_MSG("Write r0x11 = 0x%x", reg);
	Ds90ub91x_RegWR(0x12, reg, 1, 0);
	DEBUG_MSG("Write r0x12 = 0x%x", reg);
#ifdef CONFIG_BSP_LIBRA
	reg = 0x01; //GP0
	Ds90ub91x_RegWR(0x0d, reg, 1, 0);
	
	AmbaKAL_TaskSleep(10);

	reg = 0x09; //GP0
	Ds90ub91x_RegWR(0x0d, reg, 1, 0);	
#endif

	DEBUG_MSG("init ds90ub913q done!");

	return 0;
}

/**

 * Initilzation function for Sbridge
 */
static int Ds90ub91x_Config (void *arg1, void *arg2)
{
	int rval = 0;
	UINT16 sub_module_id;
	UINT16 device_slave_id = 0;

	sub_module_id = *((UINT16 *) arg1);

    if (arg2 != NULL)
		device_slave_id = *((UINT16 *) arg2);

	switch (sub_module_id) {
	case DS90UB91X_INIT :
		/* init (1) ds90ub914q, (2) ds90ub913q */
		if (Ds90ub91x_Init914q(device_slave_id) != 0 || Ds90ub91x_Init913q() != 0)
			rval = -1;
		break;

	case DS90UB91X_SELECT_914Q :
		G_ds90ub91x.slave_id_w = IDC_ADR_DS90UB914Q_W;
		G_ds90ub91x.slave_id_r = IDC_ADR_DS90UB914Q_R;
		break;
	case DS90UB91X_SELECT_913Q :
		G_ds90ub91x.slave_id_w = IDC_ADR_DS90UB913Q_W;
		G_ds90ub91x.slave_id_r = IDC_ADR_DS90UB913Q_R;
		break;
	}

        return rval;
}


/**
 * Reset the Sbridge
 */
static int Ds90ub91x_Reset(void *arg)
{
	UINT16 reg;
	G_ds90ub91x.slave_id_w = IDC_ADR_DS90UB914Q_W;
	G_ds90ub91x.slave_id_r = IDC_ADR_DS90UB914Q_R;

	reg = 0x21; //Reset
	Ds90ub91x_RegWR(0x01, reg, 1, 0);
	DEBUG_MSG("Write r0x01 = 0x%x", reg);

	return 0;
}

/**
 * Write the registers via IDC
 */
static int Ds90ub91x_RegWrite(UINT16 addr, UINT16 data, UINT16 size)
{
	Ds90ub91x_RegWR(addr, data, size, 0);

	return 0;
}

/**
 * Write the registers via IDC burst mode
 */
static int Ds90ub91x_RegBurstWrite(UINT16 addr, UINT16 data, UINT16 size)
{
	Ds90ub91x_RegWR(addr, data, size, 1);

	return 0;
}

/**
 * Read the register value via IDC
 */
static int Ds90ub91x_RegRead(UINT16 addr, UINT8 *data, UINT16 size)
{
	Ds90ub91x_RegRD(addr, data, size);

	return 0;
}

#if 0
/**
 * Show the information of S2
 */
static int ds90ub91x_info(struct sbrg_dev_s *dev)
{
	return 0;
}
#endif

/**
 * Initial device driver
 */
void Ds90ub91x_DrvDevInit(void)
{
	AMBA_SBRG_DEV_s Dev;
    //char* Name = "DS90UB91X";

	strcpy(Dev.name,"DS90UB91X");
	//Dev.name = Name;
    Dev.write_reg		= Ds90ub91x_RegWrite;
	Dev.burst_write_reg	= Ds90ub91x_RegBurstWrite;
    Dev.read_reg		= Ds90ub91x_RegRead;
	Dev.reset		    = Ds90ub91x_Reset;
	Dev.config		    = Ds90ub91x_Config;

    /* Attache the driver to the S bridge control manager */
    AmbaSbrg_DriverAttach(SBRG_CHAN_ID_DS90UB91X, &Dev);

}

