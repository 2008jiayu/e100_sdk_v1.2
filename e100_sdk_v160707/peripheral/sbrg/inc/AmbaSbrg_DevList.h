#ifndef __SBRIDGE_DEV_LIST_H__
#define __SBRIDGE_DEV_LIST_H__

__BEGIN_C_PROTO__

/**
 * Initilzation of default SBridge device driver at the system bootup.
 * The default device driver is selected menually at menuconfig
 *
 * @return - 0 successful, < 0 failed
 */
extern int AmbaSbrg_DriverInit(void);


/**********************************************/
/* APIs to initiate SBridge device drivers    */
/**********************************************/

#ifdef CONFIG_SBRIDGE_S2
extern void sbrg_drv_init_s2(void);
#endif

#ifdef CONFIG_SBRIDGE_S3D
extern void sbrg_drv_init_s3d(void);
#endif

#ifdef CONFIG_SBRIDGE_TI913914
extern void Ds90ub91x_DrvDevInit(void);
#endif

__END_C_PROTO__

#endif /* __SBRIDGE_DEV_LLIST_H__*/
