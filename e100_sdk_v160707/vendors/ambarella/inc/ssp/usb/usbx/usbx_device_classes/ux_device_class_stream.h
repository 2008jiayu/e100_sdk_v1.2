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
/**   Device Data Pump Class                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    ux_device_class_stream.h                             PORTABLE C      */ 
/*                                                           5.6          */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    Thierry Giron, Express Logic Inc.                                   */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file contains all the header and extern functions used by the  */
/*    USBX device stream class.                                            */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  07-01-2007     TCRG                     Initial Version 5.0           */ 
/*  11-11-2008     TCRG                     Modified comment(s), added    */ 
/*                                            data pump class structure,  */ 
/*                                            and added read/write APIs,  */ 
/*                                            resulting in  version 5.2   */ 
/*  07-10-2009     TCRG                     Modified comment(s), and      */ 
/*                                            added trace logic,          */ 
/*                                            resulting in version 5.3    */ 
/*  06-13-2010     TCRG                     Modified comment(s),          */ 
/*                                            resulting in version 5.4    */ 
/*  09-01-2011     TCRG                     Modified comment(s),          */ 
/*                                            resulting in version 5.5    */ 
/*  10-10-2012     TCRG                     Modified comment(s), and      */ 
/*                                            allow for change of         */ 
/*                                            alternate settings,         */ 
/*                                            resulting in version 5.6    */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef UX_DEVICE_CLASS_STREAM_H
#define UX_DEVICE_CLASS_STREAM_H

/* Define Storage Class USB Class constants.  */

#define UX_SLAVE_CLASS_STREAM_CLASS                              0xFF

#define UX_SLAVE_CLASS_STREAM_SUBCLASS                           0xFE 

#define UX_SLAVE_CLASS_STREAM_PROTOCOL                           0x00

/* Define Data Pump Class packet equivalences.  */
#define UX_DEVICE_CLASS_STREAM_PACKET_SIZE                       128


/* Define Slave STREAM Class Calling Parameter structure */

typedef struct UX_SLAVE_CLASS_STREAM_PARAMETER_STRUCT
{
    VOID                    (*ux_slave_class_stream_instance_activate)(VOID *);
    VOID                    (*ux_slave_class_stream_instance_deactivate)(VOID *);

} UX_SLAVE_CLASS_STREAM_PARAMETER;

/* Define Slave Data Pump Class structure.  */

typedef struct UX_SLAVE_CLASS_STREAM_STRUCT
{
    UX_SLAVE_INTERFACE                  *ux_slave_class_stream_interface;
    UX_SLAVE_CLASS_STREAM_PARAMETER      ux_slave_class_stream_parameter;
    UX_SLAVE_ENDPOINT                   *ux_slave_class_stream_bulkin_endpoint;
    UX_SLAVE_ENDPOINT                   *ux_slave_class_stream_bulkout_endpoint;
    ULONG                               ux_slave_class_stream_alternate_setting;
    INT                                 stream_active;

} UX_SLAVE_CLASS_STREAM;

/* Define Device Data Pump Class prototypes.  */

UINT    _ux_device_class_stream_initialize(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_stream_activate(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_stream_deactivate(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_stream_entry(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_stream_read(UX_SLAVE_CLASS_STREAM *stream, UCHAR *buffer, 
                                ULONG requested_length, ULONG *actual_length, ULONG timeout);
UINT    _ux_device_class_stream_write(UX_SLAVE_CLASS_STREAM *stream, UCHAR *buffer, 
                                ULONG requested_length, ULONG *actual_length, ULONG timeout);
UINT    _ux_device_class_stream_change(UX_SLAVE_CLASS_COMMAND *command);

UINT    _ux_device_class_stream_initialize_native(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_stream_activate_native(UX_SLAVE_CLASS_COMMAND *command);
UINT    _ux_device_class_stream_entry_native(UX_SLAVE_CLASS_COMMAND *command);


#endif
