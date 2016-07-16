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
/**   Utility                                                             */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  COMPONENT DEFINITION                                   RELEASE        */
/*                                                                        */
/*    ux_utility.h                                        PORTABLE C      */
/*                                                           5.6          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Thierry Giron, Express Logic Inc.                                   */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains all the header and extern functions used by the  */
/*    USBX components that utilize utility functions.                     */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  07-01-2007     TCRG                     Initial Version 5.0           */
/*  11-11-2008     TCRG                     Modified comment(s), and      */
/*                                            added new prototypes,       */
/*                                            resulting in version 5.2    */
/*  07-10-2009     TCRG                     Modified comment(s), and      */
/*                                            added trace logic,          */
/*                                            resulting in version 5.3    */
/*  06-13-2010     TCRG                     Modified comment(s),          */
/*                                            resulting in version 5.4    */
/*  09-01-2011     TCRG                     Modified comment(s),          */
/*                                            resulting in version 5.5    */
/*  10-10-2012     TCRG                     Modified comment(s),          */
/*                                            resulting in version 5.6    */
/*                                                                        */
/**************************************************************************/

#ifndef UX_UTILITY_H
#define UX_UTILITY_H


/* Define Utility component function prototypes.  */

VOID    _ux_utility_descriptor_parse(UCHAR * raw_descriptor, UCHAR * descriptor_structure,
                                     UINT descriptor_entries, UCHAR * descriptor);
VOID    _ux_utility_descriptor_pack(UCHAR * descriptor, UCHAR * descriptor_structure,
                                    UINT descriptor_entries, UCHAR * raw_descriptor);
ULONG   _ux_utility_long_get(UCHAR * address);
VOID    _ux_utility_long_put(UCHAR * address, ULONG value);
VOID    _ux_utility_long_put_big_endian(UCHAR * address, ULONG value);
ULONG   _ux_utility_long_get_big_endian(UCHAR * address);
VOID   *_ux_utility_memory_allocate(ULONG memory_alignment,ULONG memory_cache_flag, ULONG memory_size_requested);
UINT    _ux_utility_memory_compare(VOID *memory_source, VOID *memory_destination, ULONG length);
VOID    _ux_utility_memory_copy(VOID *memory_destination, VOID *memory_source, ULONG length);
VOID    _ux_utility_memory_free(VOID *memory);
ULONG   _ux_utility_string_length_get(UCHAR *string);
UX_MEMORY_BLOCK *_ux_utility_memory_free_block_best_get(ULONG memory_cache_flag, ULONG memory_size_requested);
VOID    _ux_utility_memory_set(VOID *destination, UCHAR value, ULONG length);
UINT    _ux_utility_mutex_create(TX_MUTEX *mutex, CHAR *mutex_name);
UINT    _ux_utility_mutex_delete(TX_MUTEX *mutex);
VOID    _ux_utility_mutex_off(TX_MUTEX *mutex);
VOID    _ux_utility_mutex_on(TX_MUTEX *mutex);
UINT    _ux_utility_mutex_on_ex(TX_MUTEX *mutex, ULONG wait_option);
ULONG   _ux_utility_pci_class_scan(ULONG pci_class, ULONG bus_number, ULONG device_number,
                                   ULONG function_number, ULONG *current_bus_number,
                                   ULONG *current_device_number, ULONG *current_function_number);
ULONG   _ux_utility_pci_read(ULONG bus_number, ULONG device_number, ULONG function_number,
                             ULONG offset, UINT read_size);
VOID    _ux_utility_pci_write(ULONG bus_number, ULONG device_number, ULONG function_number,
                              ULONG offset, ULONG value, UINT write_size);
VOID   *_ux_utility_physical_address(VOID *virtual_address);
UINT    _ux_utility_semaphore_create(TX_SEMAPHORE *semaphore, CHAR *semaphore_name, UINT initial_count);
UINT    _ux_utility_semaphore_delete(TX_SEMAPHORE *semaphore);
UINT    _ux_utility_semaphore_get(TX_SEMAPHORE *semaphore, ULONG wait_option);
UINT    _ux_utility_semaphore_put(TX_SEMAPHORE *semaphore);
VOID    _ux_utility_set_interrupt_handler(UINT irq, VOID (*interrupt_handler)(VOID));
ULONG   _ux_utility_short_get(UCHAR * address);
ULONG   _ux_utility_short_get_big_endian(UCHAR * address);
VOID    _ux_utility_short_put(UCHAR * address, USHORT value);
VOID    _ux_utility_short_put_big_endian(UCHAR * address, USHORT value);
UINT    _ux_utility_thread_create(TX_THREAD *thread_ptr, CHAR *name,
                                  VOID (*entry_function)(ULONG), ULONG entry_input,
                                  VOID *stack_start, ULONG stack_size,
                                  UINT priority, UINT preempt_threshold,
                                  ULONG time_slice, UINT auto_start);
UINT    _ux_utility_thread_delete(TX_THREAD *thread_ptr);
VOID    _ux_utility_thread_relinquish(VOID);
UINT    _ux_utility_thread_resume(TX_THREAD *thread_ptr);
UINT    _ux_utility_thread_sleep(ULONG ticks);
UINT    _ux_utility_thread_suspend(TX_THREAD *thread_ptr);
UINT    _ux_utility_timer_create(TX_TIMER *timer, CHAR *timer_name, VOID (*expiration_function) (ULONG),
                                 ULONG expiration_input, ULONG initial_ticks, ULONG reschedule_ticks,
                                 UINT activation_flag);
UINT    _ux_utility_timer_delete(TX_TIMER *timer);
UINT    _ux_utility_timer_change(TX_TIMER *timer, UINT32 initial_ticks, UINT32 reschedule_ticks, UINT32 activation_flag);
UINT    _ux_utility_timer_start(TX_TIMER *timer);
UINT    _ux_utility_timer_stop(TX_TIMER *timer);

VOID    *_ux_utility_virtual_address(VOID *physical_address);
UINT    _ux_utility_event_flags_create(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR *name);
UINT    _ux_utility_event_flags_delete(TX_EVENT_FLAGS_GROUP *group_ptr);
UINT    _ux_utility_event_flags_get(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG requested_flags,
                                    UINT get_option, ULONG *actual_flags_ptr, ULONG wait_option);
UINT    _ux_utility_event_flags_set(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG flags_to_set,
                                    UINT set_option);

VOID    _ux_utility_unicode_to_string(UCHAR *source, UCHAR *destination);
VOID    _ux_utility_string_to_unicode(UCHAR *source, UCHAR *destination);
UINT    _ux_utility_get_system_memory(VOID **base, VOID **alignedBase, UINT size, UINT alignment);
UINT    _ux_utility_set_system_cached_memory_pool(TX_BYTE_POOL *scmp);
UINT    _ux_utility_free_system_memory(VOID *memory);
void    _ux_utility_print(const char *fmt, ...);
UINT32  _ux_utility_print_destroy(void);
UINT32  _ux_utility_print_init(void);
UINT32  _ux_utility_print_kernel_enable(UINT32 value);
INT     _ux_utility_string_compare(const char *Str1, const char *Str2, UINT32 Len);
void    _ux_utility_cache_invalidate(void *address, int size);
void    _ux_utility_cache_flush(void *address, int size);
UINT    _ux_utility_thread_create_ex(TX_THREAD *thread_ptr,
										CHAR *name,
										VOID (*entry_function)(ULONG),
										ULONG entry_input,
										VOID *stack_start,
										ULONG stack_size,
										UINT priority,
										UINT preempt_threshold,
										ULONG time_slice,
										UINT auto_start,
										UINT affinity);

#endif
