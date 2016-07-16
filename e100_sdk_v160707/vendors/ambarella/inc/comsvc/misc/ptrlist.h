/**
 *  @file PtrList.h
 *
 *  pointer list header
 *
 *  **History**
 *      |Date       |Name        |Comments       |
 *      |-----------|------------|---------------|
 *      |2013/04/02 |clchan      |Created        |
 *
 *
 *  @copyright 2013 Ambarella Corporation. All rights reserved.
 *                 No part of this file may be reproduced, stored in a retrieval system,
 *                 or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                 recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 * @note not thread safe, but each handler uses its own memory space
 */
#ifndef __PTRLIST_H__
#define __PTRLIST_H__
/**
 * @addtogroup Utility
 * @{
 */

#include <AmbaDataType.h>

/**
 * The structure of a pointer list handler
 */
typedef struct {
    UINT32 Count;   /**< The number of elements that the pointer list keeps */
    UINT32 Limit;   /**< The max number of elements that the pointer list can keep */
} AMP_PTR_LIST_HDLR_s;

/**
 * The config to create a pointer list
 */
typedef struct {
    UINT8 *Buffer;  /**< The buffer */
    UINT32 Size;    /**< The buffer size */
} AMP_PTR_LIST_CFG_s;

/**
 * Create a pointer list handler from a buffer.
 * @param [in] buffer The buffer
 * @param [in] size The buffer size
 * @param [out] hdlr The double pointer to get the resulted pointer list handler
 * @return 0 - OK, others - NG
 */
extern int AmpPtrList_Create(void *buffer, UINT32 size, AMP_PTR_LIST_HDLR_s **hdlr);

/**
 * Delete a pointer list handler.
 * @param [in] hdlr The pointer list handler
 * @return 0 - OK, others - NG
 */
extern int AmpPtrList_Delete(AMP_PTR_LIST_HDLR_s *hdlr);

/**
 * Add a pointer to the head of a pointer list handler.
 * @param [in,out] hdlr The pointer list handler
 * @param [in] ptr The pointer to be added
 * @return 0 - OK, others - NG
 */
extern int AmpPtrList_AddHead(AMP_PTR_LIST_HDLR_s *hdlr, void *ptr);

/**
 * Add a pointer to the tail of a pointer list handler.
 * @param [in,out] hdlr The pointer list
 * @param [in] ptr The pointer to be added
 * @return 0 - OK, others - NG
 */
extern int AmpPtrList_AddTail(AMP_PTR_LIST_HDLR_s *hdlr, void *ptr);

/**
 * Remove the nIndex-th element of a pointer list handler.
 * @param [in,out] hdlr The pointer list
 * @param [in] index The index of the element to be deleted
 * @return 0 - OK, others - NG
 */
extern int AmpPtrList_RemoveAt(AMP_PTR_LIST_HDLR_s *hdlr, UINT32 index);

/**
 * Remove all the elements of a pointer list handler.
 * @param [in] hdlr The pointer list handler
 */
extern void AmpPtrList_RemoveAll(AMP_PTR_LIST_HDLR_s *hdlr);

/**
 * Get the index-th element of a pointer list handler.
 * @param [in,out] hdlr The pointer list handler
 * @param [in] index The index of the element
 * @param [out] ptr The double pointer to get the element
 * @return 0 - OK, others - NG
 */
extern int AmpPtrList_GetAt(AMP_PTR_LIST_HDLR_s *hdlr, UINT32 index, void **ptr);

/**
 * Get the element count of a pointer list handler.
 * @param [in] hdlr The pointer list handler
 * @return the element count
 */
extern UINT32 AmpPtrList_GetCount(AMP_PTR_LIST_HDLR_s *hdlr);

/**
 * Get the required buffer size to create a pointer list handler.
 * @param [in] maxElement The max number of elements that the pointer list handler can keep
 * @return the required buffer size (byte)
 */
extern UINT32 AmpPtrList_GetRequiredBufferSize(UINT32 maxElement);

/**
 * @}
 */

#endif

