/**
 * @file Json_Utility.h
 *
 * Copyright (C) 2015, Ambarella, Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of Ambarella, Inc.
 */

#ifndef JSON_UTILITY_H_
#define JSON_UTILITY_H_

/**
 * @defgroup Json Lib
 * @brief RTOS MW Json library module
 *
 * JsonLib is used to output JSON formatted strings and parse JSON formatted strings back into JSON objects programs.\n
 * With JsonLib, convert JSON formatted strings and JSON objects between each other.\n
 *
 */

#define JSON_ALIGNED_SIZE 32
#define MAX_JSONARROBJ_SIZE 256
#define MAX_JSONSTRING_SIZE 1024
typedef unsigned char JBOOL;

/**
 * JsonLib error code
 */
typedef enum _AMP_JSON_ERROR_e_ {
    AMP_JSON_UNKNOWN_ERROR = -1,              /**< Unknown Error */
    AMP_JSON_INVALID_PARAM = -2,                 /**< Invalid Parameter */
    AMP_JSON_INVALID_OPERATION = -3,         /**< Invalid Operation */
    AMP_JSONL_NO_MORE_MEMORY = -4           /**< Run out of Memory */
} AMP_JSON_ERROR_e;

/**
 * JsonLib supported object type
 */
typedef enum _AMP_JSON_TYPE_e_ {
    AMP_JSON_TYPE_BOOLEAN,      /**< boolean type */
    AMP_JSON_TYPE_DOUBLE,         /**< double type */
    AMP_JSON_TYPE_INTEGER,        /**< integer type */
    AMP_JSON_TYPE_STRING,          /**< string type */
    AMP_JSON_TYPE_ARRAY,           /**< array type */
    AMP_JSON_TYPE_OBJECT,          /**< object type */
    AMP_JSON_TYPE_NONE              /**< unkown type */
} AMP_JSON_TYPE_e;

/**
 * JsonLib default init config
 */
typedef struct _AMP_JSON_INIT_CFG_s_ {
    void* MemPoolAddr;              /**< memory address */
    unsigned int MemPoolSize;      /**< memory size */
} AMP_JSON_INIT_CFG_s;

/**
 * JsonLib the aligned and raw memory address of json string
 */
typedef struct _AMP_JSON_STRING_s_ {
    char JsonString[1024];      /**< Aligned Address of memory pool */
    void *RawBaseAddr;         /**< Raw Address of memory pool */
} AMP_JSON_STRING_s;

/**
 * JsonLib json object
 */
typedef struct _AMP_JSON_OBJECT_s_
{
    AMP_JSON_TYPE_e ObjType;                             /**< json object type */
    void *RawBaseAddr;                                         /**< raw address of json object */
    char ObjKey[32];                                               /**< key of json object */
    union _OBJVALUE_U_{
        JBOOL Boolean;                                              /**< json value with boolean */
        double Double;                                                /**< json value with double */
        int Integer;                                                     /**< json value with integer */
        char String[512];                                             /**< json value with string */
        struct _AMP_JSON_OBJECT_s_ *Array;          /**< json value with array */
        struct _AMP_JSON_OBJECT_s_ *Object;        /**< json value with object */
    } ObjValue;
    struct _AMP_JSON_OBJECT_s_ *ObjNext;          /**< pointer to json object */
} AMP_JSON_OBJECT_s;

/**
 * JsonLib json string command
 */
typedef struct _AMP_JSON_CMD_s_ {
    unsigned int ParamSize;         /**< Message size */
    unsigned int MsgId;                /**< Message id */
    char Param[1024];                /**< Message contents */
} AMP_JSON_CMD_s;

/**
 * Free json string for JsonLib
 *
 * @param [in] jsonString the info of json string and raw address
 * @see AMP_JSON_STRING_s
 *
 * @return 0 - OK, others - AMP_JSON_ERROR_e
 */
extern int AmpJson_FreeJsonString(AMP_JSON_STRING_s *jsonString);

/**
 * Free json object for JsonLib
 *
 * @param [in] jsonObject the info of json object
 * @see AMP_JSON_OBJECT_s
 *
 * @return 0 - OK, others - AMP_JSON_ERROR_e
 */
extern int AmpJson_FreeJsonObject(AMP_JSON_OBJECT_s *jsonObject);

/**
 * Get default configuration for JsonLib initial.
 *
 * @param [in] defaultCfg the default init config
 *
 * @return 0 - OK, -1 - AMP_JSON_ERROR_e
 * @see AMP_JSON_INIT_CFG_s
 * @see AMP_JSON_ERROR_e
 */
extern int AmpJson_GetInitDefaultCfg(AMP_JSON_INIT_CFG_s *defaultCfg);

/**
 * JsonLib initial function,
 *
 * @param [in] initCfg the init config
 *
 * @return 0 -OK, -1 - AMP_JSON_ERROR_e
 * @see AMP_JSON_INIT_CFG_s
 * @see AMP_JSON_ERROR_e
 */
extern int AmpJson_Init(AMP_JSON_INIT_CFG_s *initCfg);

/**
 * Convert json string to json object for JsonLib,
 *
 * @param [in] jsonString the json string
 *
 * @return pointer to json object - OK, NULL - NG
 * @see AMP_JSON_OBJECT_s
 */
extern AMP_JSON_OBJECT_s* AmpJson_JsonStringToJsonObject(const char *jsonString);

/**
 * Get string by key with json object for JsonLib,
 *
 * @param [in] jsonObject the head of json object
 * @param [in] objKey the specific key string
 * @param [out] retString the value with string
 * @param [in] retMaxLen the max size of string to retString
 *
 * @return 0 - OK, others - AMP_JSON_ERROR_e
 */
extern int AmpJson_GetStringByKey(AMP_JSON_OBJECT_s *jsonObject, const char *objKey, char *retString, int retMaxLen);

/**
 * Get integer by key with json object for JsonLib,
 *
 * @param [in] jsonObject the head of json object
 * @param [in] objKey the specific key string
 * @param [out] retInt the value with integer
 *
 * @return 0 - OK, others - AMP_JSON_ERROR_e
 */
extern int AmpJson_GetIntegerByKey(AMP_JSON_OBJECT_s *jsonObject, const char *objKey, int *retInt);

/**
 * Get json object by key for JsonLib,
 *
 * @param [in] jsonObject the head of json object
 * @param [in] objKey the specific key string
 *
 * @return pointer to json object - OK, NULL - NG
 * @see AMP_JSON_OBJECT_s
 */
extern AMP_JSON_OBJECT_s* AmpJson_GetObjectByKey(AMP_JSON_OBJECT_s *jsonObject, const char *objKey);

/**
 * Get json type with json object for JsonLib,
 *
 * @param [in] jsonObject the specific json object
 *
 * @return AMP_JSON_TYPE_e - OK, others - AMP_JSON_ERROR_e
 * @see AMP_JSON_TYPE_e
 */
extern AMP_JSON_TYPE_e AmpJson_GetObjectType(AMP_JSON_OBJECT_s *jsonObject);

/**
 * Get integer with json object for JsonLib,
 *
 * @param [in] jsonObject the specific json object
 *
 * @return value with integer -OK, others - AMP_JSON_ERROR_e
 * @see AMP_JSON_TYPE_e
 */
extern int AmpJson_GetInteger(AMP_JSON_OBJECT_s *jsonObject);

/**
 * Get string with json object for JsonLib,
 *
 * @param [in] jsonObject the specific json object
 *
 * @return value with string -OK, others - AMP_JSON_ERROR_e
 * @see AMP_JSON_TYPE_e
 */
extern char* AmpJson_GetString(AMP_JSON_OBJECT_s *jsonObject);

/**
 * Get boolean with json object for JsonLib,
 *
 * @param [in] jsonObject the specific json object
 *
 * @return value with boolean -OK, others - AMP_JSON_ERROR_e
 * @see AMP_JSON_TYPE_e
 */
extern JBOOL AmpJson_GetBoolean(AMP_JSON_OBJECT_s *jsonObject);

/**
 * Get double with json object for JsonLib,
 *
 * @param [in] jsonObject the specific json object
 *
 * @return value with double -OK, others - AMP_JSON_ERROR_e
 * @see AMP_JSON_TYPE_e
 */
extern double AmpJson_GetDouble(AMP_JSON_OBJECT_s *jsonObject);

/**
 * Add key and value to json object for JsonLib,
 *
 * @param [in] jsonObject the head of json object
 * @param [in] objKey the key with string
 * @param [in] jsonObjValue the value with json object
 *
 */
extern void AmpJson_AddObject(AMP_JSON_OBJECT_s *jsonObject, char *objKey, AMP_JSON_OBJECT_s *jsonObjValue);

/**
 * Append json object to array for JsonLib,
 *
 * @param [in] jsonObject the head of json array
 * @param [in] jsonObjValue the specific json object
 *
 * @return 0 - OK, others - AMP_JSON_ERROR_e
 * @see AMP_JSON_TYPE_e
 */
extern int AmpJson_AddObjectToArray(AMP_JSON_OBJECT_s *jsonObject, AMP_JSON_OBJECT_s *jsonObjValue);

/**
 * Create the head of json object for JsonLib,
 *
 * @param [in] void
 *
 * @return pointer to json object - OK, NULL - NG
 * @see AMP_JSON_OBJECT_s
 */
extern AMP_JSON_OBJECT_s* AmpJson_CreateObject(void);

/**
 * Create the head of array object for JsonLib,
 *
 * @param [in] void
 *
 * @return pointer to array object - OK, NULL - NG
 * @see AMP_JSON_OBJECT_s
 */
extern AMP_JSON_OBJECT_s* AmpJson_CreateArrayObject(void);

/**
 * Create string object for JsonLib,
 *
 * @param [in] string the value with string
 *
 * @return pointer to json object - OK, NULL - NG
 * @see AMP_JSON_OBJECT_s
 */
extern AMP_JSON_OBJECT_s* AmpJson_CreateStringObject(char *string);

/**
 * Create string object with length for JsonLib,
 *
 * @param [in] string the value with string
 * @param [in] size the number of characters to be copied from pString
 *
 * @return pointer to json object - OK, NULL - NG
 * @see AMP_JSON_OBJECT_s
 */
extern AMP_JSON_OBJECT_s* AmpJson_CreateStringObjectWithLen(char *string, unsigned int size);

/**
 * Create integer object for JsonLib,
 *
 * @param [in] value the value with integer
 *
 * @return pointer to json object - OK, NULL - NG
 * @see AMP_JSON_OBJECT_s
 */
extern AMP_JSON_OBJECT_s* AmpJson_CreateIntegerObject(int value);

/**
 * Covert json object to json string for JsonLib,
 *
 * @param [in] jsonObject the head of json object
 *
 * @return pointer to AMP_JSON_STRING_s -OK, NULL - NG
 * @see AMP_JSON_STRING_s
 */
extern AMP_JSON_STRING_s* AmpJson_JsonObjectToJsonString(AMP_JSON_OBJECT_s *jsonObject);

/**
 * covert json object to json string function with flag,
 *
 * @param [in] obj pointer to json object
 * @param [in] flags value
 *
 * @return pointer to json string -OK, NULL - NG
 * @see AMP_JSON_STRING_s
 */
extern AMP_JSON_STRING_s* AmpJson_ObjectToStringExt(AMP_JSON_OBJECT_s *obj, int flags);

/**
 * set serializer function,
 *
 * @param [in] ptr pointer to void
 *
 */
extern void AmpJson_SetSerializer(void *ptr);

/**
 * get json object,
 *
 * @param [in] obj pointer to json object
 *
 * @return pointer to json object -OK, NULL - NG
 * @see AMP_JSON_OBJECT_s
 */
extern AMP_JSON_OBJECT_s* AmpJson_GetObject(AMP_JSON_OBJECT_s *obj);

/**
 * put json object,
 *
 * @param [in] obj pointer to json object
 *
 * @return 0 - OK, -1 - NG
 */
extern int AmpJson_PutObject(AMP_JSON_OBJECT_s *obj);

/**
 * check json type is valid,
 *
 * @param [in] obj pointer to json object
 * @param [in] json type
 *
 * @return 0 - OK, -1 - NG
 */
extern int AmpJson_IsObjectType(AMP_JSON_OBJECT_s *obj, AMP_JSON_TYPE_e type);

/**
 * get json object length,
 *
 * @param [in] obj pointer to json object
 *
 * @return 0 - OK, -1 - NG
 */
extern int AmpJson_GetObjectLength(AMP_JSON_OBJECT_s *obj);

/**
 * get json object with key and value,
 *
 * @param [in] obj pointer to json object
 * @param [in] key pointer to string
 * @param [in] value pointer to pointer to json object
 *
 * @return 0 - OK, -1 - NG
 */
extern JBOOL AmpJson_GetObjectEx(AMP_JSON_OBJECT_s *obj, const char *key, AMP_JSON_OBJECT_s **value);

/**
 * delete json object with key,
 *
 * @param [in] obj pointer to json object
 * @param [in] key pointer to string
 *
 */
extern void AmpJson_DeleteObject(AMP_JSON_OBJECT_s *obj, const char *key);

/**
 * get json array,
 *
 * @param [in] obj pointer to json object
 *
 * @return pointer to json object -OK, NULL - NG
 * @see AMP_JSON_OBJECT_s
 */
extern AMP_JSON_OBJECT_s* AmpJson_GetArray(AMP_JSON_OBJECT_s *obj);

/**
 * get json length of array,
 *
 * @param [in] obj pointer to json object
 *
 * @return >0 - OK, -1 - NG
 */
extern int AmpJson_GetArrayLength(AMP_JSON_OBJECT_s *obj);

/**
 * sort json array,
 *
 * @param [in] jso pointer to json object
 * @param [in] cbsort pointer to sorting function
 *
 */
extern void AmpJson_SortArray(AMP_JSON_OBJECT_s *jso, int(*cbsort)(const void *, const void *));

/**
 * put json object to array with index,
 *
 * @param [in] obj pointer to json array
 * @param [in] index of json array
 * @param [in] val pointer to json object
 *
 * @return 0 - OK, -1 - NG
 */
extern int AmpJson_PutObjectToArrayWithIndex(AMP_JSON_OBJECT_s *obj, int idx, AMP_JSON_OBJECT_s *val);

/**
 * get json object to array with index,
 *
 * @param [in] obj pointer to json array
 * @param [in] index of json array
 *
 * @return pointer to json object - OK, NULL - NG
 * @see AMP_JSON_OBJECT_s
 */
extern AMP_JSON_OBJECT_s* AmpJson_GetObjectFromArrayWithIndex(AMP_JSON_OBJECT_s *obj, int idx);

/**
 * create json object with integer64
 *
 * @param [in] Integer64 value
 *
 * @return pointer to json object -OK, NULL - NG
 * @see AMP_JSON_OBJECT_s
 */
extern AMP_JSON_OBJECT_s* AmpJson_CreateInteger64Object(signed long long int value);

/**
 * get integer64 by json object,
 *
 * @param [in] obj pointer to json object
 *
 * @return integer64 value - OK, -1 - NG
 */
extern signed long long int AmpJson_GetInteger64(AMP_JSON_OBJECT_s *obj);

/**
 * get string length,
 *
 * @param [in] obj pointer to json object
 *
 * @return >0 - OK, -1 - NG
 */
extern int AmpJson_GetStringLength(AMP_JSON_OBJECT_s *obj);

#endif /* JSON_UTILITY_H_ */

