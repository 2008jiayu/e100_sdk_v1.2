 /**
  * @file wchar.c
  *
  * SDK UINT16 utilities
  *
  * History:
  *    2014/11/19 - [Jenghung Luo] created file
  *
  * Copyright (C) 2014, Ambarella, Inc.
  *
  * All rights reserved. No Part of this file may be reproduced, stored
  * in a retrieval system, or transmitted, in any form, or by any means,
  * electronic, mechanical, photocopying, recording, or otherwise,
  * without the prior consent of Ambarella, Inc.
  */

#include "w_char.h"
#include "AmbaUtility.h"

/**
 * Get WCHAR string length
 *
 * @param [in] str input string
 *
 * @return size_t size of the string
 */
size_t w_strlen(const WCHAR *str)
{
    const WCHAR* i;

    for (i = str; *i != 0x0000; i++);

    return (size_t)(i - str);
}

/**
 * WCHAR string copy
 *
 * @param [out] dst destination
 * @param [in] src source
 *
 * @return dst pointer
 */
WCHAR* w_strcpy(WCHAR *dst, const WCHAR *src)
{
    WCHAR* d = dst;

    while ((*d++ = *src++) != 0x0000);

    return dst;
}

/**
 * WCHAR string copy by length
 *
 * @param [out] dst destination
 * @param [in] src source
 * @param [in] len number of WCHAR to copy
 *
 * @return destination
 */
WCHAR* w_strncpy(WCHAR *dst, const WCHAR *src, size_t len)
{
    WCHAR* d = dst;

    while ((len > 0) && (*src != 0x0000)) {
        *d++ = *src++;
        len --;
    }
    if (len > 0) {
        *d = 0x0000;
    }

    return dst;
}

/**
 * WCHAR string comparison
 *
 * @param [in] s1 string #1
 * @param [in] s2 string #2
 *
 * @return distance
 */
int w_strcmp(const WCHAR *s1, const WCHAR *s2)
{
    while ((*s1 != 0x0000) && (*s2 != 0x0000) && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}

/**
 * WCHAR string comparison by length
 *
 * @param [in] s1 string #1
 * @param [in] s2 string #2
 * @param [in] len search length
 *
 * @return 0 - equal, others - first difference position
 */
int w_strncmp(const WCHAR *s1, const WCHAR *s2, size_t len)
{
    while (len > 0) {
        if ((*s1 == 0x0000) || (*s2 == 0x0000) || (*s1 != *s2)) {
            return (*s1 - *s2);
        }
        s1++;
        s2++;
        len --;
    }
    return 0;
}


/**
 * Find the first WCHAR occurrence in a string
 *
 * @param [in] src source string
 * @param [in] ch WCHAR to find
 *
 * @return position
 */
WCHAR* w_strchr(const WCHAR *src, WCHAR ch)
{
    while ((*src != ch) && (*src != 0x0000)) {
        src++;
    }
    if (*src == ch) {
        return (WCHAR*) src;
    }
    return NULL;
}

/**
 * Find the last WCHAR occurrence in a string
 *
 * @param [in] src source string
 * @param [in] ch WCHAR to find
 *
 * @return position
 */
WCHAR* w_strrchr(const WCHAR *src, WCHAR ch)
{
    const WCHAR *last;

    last = NULL;
    while(1) {
        if (*src == ch)
            last = src;
        if (*src == 0x0000)
            break;
        src++;
    }

    return (WCHAR *)last;
}


/**
 * Append WCHAR string
 *
 * @param [out] dest destination
 * @param [in] src source string
 *
 * @return destination
 */
WCHAR* w_strcat(WCHAR *dst, const WCHAR *src)
{
    WCHAR* d = dst;

    while (*d != 0x0000) {
        d++;
    }
    while (*src != 0x0000) {
        *d++ = *src++;
    }
    *d = 0x0000;
    return dst;
}

/**
 * convert WCHAR to byte
 *
 * @param [out] dest destination
 * @param [in] src source string
 * @param [in] len length
 *
 * @return none
 */
void w_uni2asc(char *dst, const WCHAR *src, size_t len)
{
    AmbaUtility_Unicode2Ascii(src, dst);
    return;
}

/**
 * convert byte to WCHAR
 *
 * @param [out] dest destination
 * @param [in] src source string
 * @param [in] len length
 *
 * @return none
 */
void w_asc2uni(WCHAR *dst, const char *src, size_t len)
{
    AmbaUtility_Ascii2Unicode(src, dst);
    return;
}

/**
 * extract tokens from strings
 *
 * @param [out] dest destination
 * @param [in] src source string
 * @param [in] len length
 *
 * @return pointer to next token
 */
WCHAR* w_strtok(WCHAR *s, const WCHAR *delim, WCHAR **last)
{
    const WCHAR *spanp;
    WCHAR *token, c, sc;

    if ((s == NULL) && ((s = *last) == NULL)) {
        return (NULL);
    }
cont:
    c = *s++;
    for (spanp = delim; (sc = *spanp++) != 0x0000;) {
        if (c == sc)
            goto cont;
    }

    if (c == 0x0000) {
        *last = NULL;
        return (NULL);
    }
    token = s - 1;

    for (;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0x0000)
                    s = NULL;
                else
                    s[-1] = 0x0000;
                *last = s;
                return (token);
            }
        } while (sc != 0x0000);
    }

}

/**
 * Find the first string occurrence in a string
 *
 * @param [in] s source string
 * @param [in] needle WCHAR string to find
 *
 * @return position
 */
WCHAR* w_strstr(const WCHAR *s, const WCHAR *needle)
{
	WCHAR c, sc;
	size_t len;

	if ((c = *needle++) != 0) {
		len = w_strlen(needle);
		do {
			do {
				if ((sc = *s++) == 0x0000)
					return (NULL);
			} while (sc != c);
		} while (w_strncmp(s, needle, len) != 0);
		s--;
	}
	return ((WCHAR *)s);
}
