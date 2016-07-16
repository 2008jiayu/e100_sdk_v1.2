#ifndef __H_APPLIB_NET_JSON_UTILITY__
#define __H_APPLIB_NET_JSON_UTILITY__

#include "AmbaDataType.h"

#ifndef NULL
#define NULL (void*)0
#endif

typedef void APPLIB_JSON_OBJECT;
typedef void APPLIB_JSON_STRING;

typedef enum _APPLIB_JSON_OBJ_TYPE_e_{
    APPLIB_JSON_OBJ_TYPE_INTEGER = 0,
    APPLIB_JSON_OBJ_TYPE_STRING,
    APPLIB_JSON_OBJ_TYPE_NUM
} APPLIB_JSON_OBJ_TYPE_e;



int AppLibNetJsonUtility_JsonStringToJsonObject(APPLIB_JSON_OBJECT **JsonObject, const char *JsonString);
int AppLibNetJsonUtility_GetIntegerByKey(APPLIB_JSON_OBJECT *JsonObject, const char *ObjKey, int *RetInt);
int AppLibNetJsonUtility_GetStringByKey(APPLIB_JSON_OBJECT *JsonObject, const char *ObjKey, char *RetString, int RetMaxLen);

int AppLibNetJsonUtility_CreateObject(APPLIB_JSON_OBJECT **JsonObject);
int AppLibNetJsonUtility_FreeJsonObject(APPLIB_JSON_OBJECT *JsonObject);
int AppLibNetJsonUtility_AddIntegerObject(APPLIB_JSON_OBJECT *JsonObject, char *Key, int Value);
int AppLibNetJsonUtility_AddStringObject(APPLIB_JSON_OBJECT *JsonObject, char *Key, char *Value);
int AppLibNetJsonUtility_AddObject(APPLIB_JSON_OBJECT *JsonObject, char *Key, APPLIB_JSON_OBJECT *JsonArrayObject);

int AppLibNetJsonUtility_CreateArrayObject(APPLIB_JSON_OBJECT **JsonObject);
int AppLibNetJsonUtility_FreeJsonArrayObject(APPLIB_JSON_OBJECT *JsonObject);
int AppLibNetJsonUtility_AddObjectToArray(APPLIB_JSON_OBJECT *JsonObject, char *String);

int AppLibNetJsonUtility_JsonObjectToJsonString(APPLIB_JSON_OBJECT *JsonObject, APPLIB_JSON_STRING **JsonString);
int AppLibNetJsonUtility_GetString(APPLIB_JSON_STRING *JsonString, char **String);
int AppLibNetJsonUtility_FreeJsonString(APPLIB_JSON_STRING *JsonString);


#endif /**/

