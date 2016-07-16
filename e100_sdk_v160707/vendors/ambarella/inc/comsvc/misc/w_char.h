 /**
  * @file wchar.h
  *
  * SDK UINT16 utilities
  *
  * History:
  *    2014/11/19 - [Jenghung Luo] created file
  *
  * Copyright (C) 2013, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#ifndef _WCHAR_H_
#define _WCHAR_H_

#include "stdlib.h"
#include "AmbaDataType.h"

/**
 * Get WCHAR string length
 *
 * @param [in] str input string
 *
 * @return size_t size of the string
 */
size_t w_strlen(const WCHAR *str);

/**
 * WCHAR string copy
 *
 * @param [out] dst destination
 * @param [in] src source
 *
 * @return dst pointer
 */
WCHAR* w_strcpy(WCHAR *dst, const WCHAR *src);

/**
 * WCHAR string copy by length
 *
 * @param [out] dst destination
 * @param [in] src source
 * @param [in] len number of WCHAR to copy
 *
 * @return destination
 */
WCHAR* w_strncpy(WCHAR *dst, const WCHAR *src, size_t len);

/**
 * WCHAR string comparison
 *
 * @param [in] s1 string #1
 * @param [in] s2 string #2
 *
 * @return distance
 */
int w_strcmp(const WCHAR *s1, const WCHAR *s2);

/**
 * WCHAR string comparison by length
 *
 * @param [in] s1 string #1
 * @param [in] s2 string #2
 * @param [in] len search length
 *
 * @return 0 - equal, others - first difference position
 */
int w_strncmp(const WCHAR *s1, const WCHAR *s2, size_t len);

/**
 * Find the first WCHAR occurrence
 *
 * @param [in] src source string
 * @param [in] ch WCHAR to find
 *
 * @return position
 */
WCHAR* w_strchr(const WCHAR *src, WCHAR ch);

/**
 * Find the last WCHAR occurrence
 *
 * @param [in] src source string
 * @param [in] ch WCHAR to find
 *
 * @return position
 */
WCHAR* w_strrchr(const WCHAR *src, WCHAR ch);

/**
 * Append WCHAR string
 *
 * @param [out] dest destination
 * @param [in] src source string
 *
 * @return destination
 */
WCHAR* w_strcat(WCHAR *dst, const WCHAR *src);

/**
 * convert WCHAR to byte
 *
 * @param [out] dest destination
 * @param [in] src source string
 * @param [in] len length
 *
 * @return none
 */
void w_uni2asc(char *dst, const WCHAR *src, size_t len);

/**
 * convert byte to WCHAR
 *
 * @param [out] dest destination
 * @param [in] src source string
 * @param [in] len length
 *
 * @return none
 */
void w_asc2uni(WCHAR *dst, const char *src, size_t len);

/**
 * extract tokens from strings
 *
 * @param [out] dest destination
 * @param [in] src source string
 * @param [in] len length
 * 
 * @return pointer to next token
 */
WCHAR* w_strtok(WCHAR *s, const WCHAR *delim, WCHAR **last);

/**
 * Find the first string occurrence in a string
 *
 * @param [in] s source string
 * @param [in] needle WCHAR string to find
 *
 * @return position
 */
WCHAR* w_strstr(const WCHAR *s, const WCHAR *needle);
#endif


