/**************************************************************************/ 
/*                                                                        */ 
/*            Copyright (c) 1996-2012 by Express Logic Inc.               */ 
/*                                                                        */ 
/*  This software is copyrighted by and is the sole property of Express   */ 
/*  Logic, Inc.  All rights, title, ownership, or other interests         */ 
/*  in the software remain the property of Express Logic, Inc.  This      */ 
/*  software may only be used in accordance with the corresponding        */ 
/*  license agreement.  Any unauthorized use, duplication, transmission,  */ 
/*  distribution, or disclosure of this software is expressly forbidden.  */ 
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */ 
/*  written consent of Express Logic, Inc.                                */ 
/*                                                                        */ 
/*  Express Logic, Inc. reserves the right to modify this software        */ 
/*  without notice.                                                       */ 
/*                                                                        */ 
/*  Express Logic, Inc.                     info@expresslogic.com         */
/*  11423 West Bernardo Court               http://www.expresslogic.com   */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   Storage Class                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    ux_host_class_iso.h                                 PORTABLE C      */ 
/*                                                           5.6          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron, Express Logic Inc.                                   */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file contains all the header and extern functions used by the  */
/*    USBX storage class.                                                 */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  07-01-2007     TCRG                     Initial Version 5.0           */ 
/*  07-04-2008     TCRG                     Modified comment(s),          */ 
/*                                            resulting in version 5.1    */ 
/*  11-11-2008     TCRG                     Modified comment(s),          */ 
/*                                            resulting in version 5.2    */ 
/*  07-10-2009     TCRG                     Modified comment(s), ensure   */ 
/*                                            the SCSI CBW and CSW buffers*/ 
/*                                            are aligned on 32bit and do */ 
/*                                            not rely on the compiler    */ 
/*                                            packing pragma some DMA     */ 
/*                                            engines of certain OHCI     */ 
/*                                            controllers (SH7727 in      */ 
/*                                            particular) requires all    */ 
/*                                            OHCI data buffers to be     */ 
/*                                            aligned on 32 bits even     */ 
/*                                            though OHCI does not demand */ 
/*                                            it, and added trace logic,  */ 
/*                                            resulting in version 5.3    */ 
/*  06-13-2010     TCRG                     Modified comment(s),          */ 
/*                                            resulting in version 5.4    */ 
/*  09-01-2011     TCRG                     Modified comment(s),          */ 
/*                                            resulting in version 5.5    */ 
/*  10-10-2012     TCRG                     Modified comment(s),          */ 
/*                                            resulting in version 5.6    */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef UX_HOST_CLASS_ISO_H
#define UX_HOST_CLASS_ISO_H


/* Include the FileX API or the FileX stub API in cases where FileX is not
   available.  */


/* Define User configurable Storage Class constants.  */

#ifndef UX_MAX_HOST_LUN
#define UX_MAX_HOST_LUN                                     16
#endif

#define USB_DEV_PRODUCT_ID_ISO_TEST         0x0005  /* product id - Iso     */
#define USB_DEV_VENDOR_ID_A7L                0x4255  /* vendor id            */

#ifndef UX_HOST_CLASS_ISO_THREAD_STACK_SIZE
#define UX_HOST_CLASS_ISO_THREAD_STACK_SIZE             UX_THREAD_STACK_SIZE
#endif

#define UX_HOST_CLASS_ISO_THREAD_PRIORITY_CLASS         70

#define UX_HOST_CLASS_ISO_THREAD_SLEEP_TIME             (10 * UX_PERIODIC_RATE)

typedef struct UX_HOST_CLASS_ISO_STRUCT
{
    struct UX_HOST_CLASS_ISO_STRUCT  
                    *ux_host_class_iso_next_instance;
    UX_HOST_CLASS   *ux_host_class_iso_class;
    UX_DEVICE       *ux_host_class_iso_device;
    UX_INTERFACE    *ux_host_class_iso_interface;
    UX_ENDPOINT     *ux_host_class_iso_iso_in_endpoint;
    UX_ENDPOINT     *ux_host_class_iso_iso_out_endpoint;
    UINT            ux_host_class_iso_state;
    UINT            (*ux_host_class_iso_transport) (struct UX_HOST_CLASS_ISO_STRUCT *iso, UCHAR * data_pointer);
    UCHAR *         ux_host_class_iso_memory;
    TX_SEMAPHORE    ux_host_class_iso_semaphore;
} UX_HOST_CLASS_ISO;

UINT  _ux_host_class_iso_entry(UX_HOST_CLASS_COMMAND *command);
VOID  _ux_host_class_iso_thread_entry(ULONG class_address);
UINT  _ux_host_class_iso_configure(UX_HOST_CLASS_ISO *iso);
UINT  _ux_host_class_iso_endpoints_get(UX_HOST_CLASS_ISO *iso);
UINT  _ux_host_class_iso_activate(UX_HOST_CLASS_COMMAND *command);
UINT  _ux_host_class_iso_deactivate(UX_HOST_CLASS_COMMAND *command);
// 2014/06/06 - remove compile warnings.
// It's not perfect to use external function in APP layer from driver.
// But at least it works.
extern UINT  AmbaUSB_Debug_WriteData(UINT Data0, UINT Data1, UINT Data2, UINT Data3, UINT Data4);

#endif

