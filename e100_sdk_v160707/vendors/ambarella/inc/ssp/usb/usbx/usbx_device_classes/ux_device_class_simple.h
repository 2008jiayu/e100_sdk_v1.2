/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   Simple Class                                                           */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#ifndef UX_DEVICE_CLASS_SIMPLE_H
#define UX_DEVICE_CLASS_SIMPLE_H

/* Define USB Simple Class constants.  */
#define UX_SLAVE_CLASS_SIMPLE_CLASS                       255

/* Device Simple Requests */
#define UX_SLAVE_CLASS_SIMPLE_SEND_COMMAND                0x00

/* Define Slave Simple Class Calling Parameter structure */

typedef struct UX_SLAVE_CLASS_SIMPLE_PARAMETER_STRUCT
{
    VOID                    (*instance_activate)(VOID *);
    VOID                    (*instance_deactivate)(VOID *);
    INT                     (*instance_vendor_request)(UINT32 RequestType, UINT32 request, UINT32 value, UINT32 index, UINT32 len);
} UX_SLAVE_CLASS_SIMPLE_PARAMETER;

/* Define Simple Class structure.  */

typedef struct UX_SLAVE_CLASS_SIMPLE_STRUCT
{
    UX_SLAVE_INTERFACE                  *interface;
    UX_SLAVE_CLASS_SIMPLE_PARAMETER     parameter;
    TX_MUTEX                            ep_in_mutex;
    TX_MUTEX                            ep_out_mutex;
} UX_SLAVE_CLASS_SIMPLE;

/* Define some Simple Class structures */

/* Define buffer length for IN/OUT pipes.  */

#define UX_SLAVE_CLASS_SIMPLE_BUFFER_SIZE                  4096

/* Define Device Simple Class prototypes.  */

UINT  _ux_device_class_simple_activate(UX_SLAVE_CLASS_COMMAND *command);
VOID  _ux_device_class_simple_control_request(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_simple_deactivate(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_simple_entry(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_simple_initialize(UX_SLAVE_CLASS_COMMAND *command);
UINT  _ux_device_class_simple_write(UX_SLAVE_CLASS_SIMPLE *simple, UCHAR *buffer, 
                                ULONG requested_length, ULONG *actual_length, ULONG endpoint_address, ULONG timeout);
UINT  _ux_device_class_simple_read(UX_SLAVE_CLASS_SIMPLE *simple, UCHAR *buffer, 
                                ULONG requested_length, ULONG *actual_length, ULONG endpoint_address, ULONG timeout);
UINT  udc_simple_reset(UX_SLAVE_CLASS_COMMAND *command);

#endif /* UX_DEVICE_CLASS_CDC_ACM_H */
