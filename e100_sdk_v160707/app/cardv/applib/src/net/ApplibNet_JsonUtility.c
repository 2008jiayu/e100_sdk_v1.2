#include <net/ApplibNet_JsonUtility.h>
#include <net/Json_Utility.h>
#include "AmbaPrintk.h"

//#define JSON_DBG_EN
#define JSON_ERR_EN

#undef JSON_DBG
#ifdef JSON_DBG_EN
#define JSON_DBG(fmt,args...) AmbaPrintColor(GREEN,fmt,##args);
#else
#define JSON_DBG(fmt,args...)
#endif

#undef JSON_ERR
#ifdef JSON_ERR_EN
#define JSON_ERR(fmt,args...) AmbaPrintColor(RED,fmt,##args);
#else
#define JSON_ERR(fmt,args...)
#endif

int AppLibNetJsonUtility_JsonStringToJsonObject(APPLIB_JSON_OBJECT **JsonObject, const char *JsonString)
{
    AMP_JSON_OBJECT_s *pstJsonObject = NULL;

    if ((!JsonObject) || (!JsonString)) {
        JSON_ERR("[AppLib - NetJsonUtility] <JsonStringToJsonObject> invalid param (JsonObject = 0x%08x, JsonString = 0x%08x)"
                                                                                ,(UINT32)JsonObject, (UINT32)JsonString);
        return -1;
    }

    *JsonObject = NULL;
    pstJsonObject = AmpJson_JsonStringToJsonObject(JsonString);
    if (!pstJsonObject) {
        JSON_ERR("[AppLib - NetJsonUtility] <JsonStringToJsonObject> AmpJson_JsonStringToJsonObject() fail");
        return -1;
    }

    *JsonObject = (APPLIB_JSON_OBJECT*)pstJsonObject;

    return 0;
}

int AppLibNetJsonUtility_GetIntegerByKey(APPLIB_JSON_OBJECT *JsonObject, const char *ObjKey, int *RetInt)
{
    AMP_JSON_OBJECT_s *pstJsonObject = (AMP_JSON_OBJECT_s *)JsonObject;
    int ReturnValue = 0;

    if ((!pstJsonObject) || (!ObjKey) || (!RetInt)) {
        JSON_ERR("[AppLib - NetJsonUtility] <GetIntegerByKey> invalid param");
        return -1;
    }

    ReturnValue = AmpJson_GetIntegerByKey(pstJsonObject, ObjKey, RetInt);
    if (ReturnValue != 0) {
         JSON_ERR("[AppLib - NetJsonUtility] <GetIntegerByKey> AmpJson_GetIntegerByKey() fail. (Err: %d)",ReturnValue);
         return -1;
    }

    JSON_DBG("[AppLib - NetJsonUtility] <GetIntegerByKey> key = %s, value = %d", ObjKey, *RetInt);

    return 0;
}

int AppLibNetJsonUtility_GetStringByKey(APPLIB_JSON_OBJECT *JsonObject, const char *ObjKey, char *RetString, int RetMaxLen)
{
    AMP_JSON_OBJECT_s *pstJsonObject = (AMP_JSON_OBJECT_s *)JsonObject;
    int ReturnValue = 0;

    if ((!pstJsonObject) || (!ObjKey) || (!RetString) || (RetMaxLen == 0)) {
        JSON_ERR("[AppLib - NetJsonUtility] <GetStringByKey> invalid param");
        return -1;
    }

    ReturnValue = AmpJson_GetStringByKey(pstJsonObject, ObjKey, RetString, RetMaxLen);
    if (ReturnValue != 0) {
         JSON_ERR("[AppLib - NetJsonUtility] <GetIntegerByKey> AmpJson_GetStringByKey() fail. (Err: %d)",ReturnValue);
         return -1;
    }

    JSON_DBG("[AppLib - NetJsonUtility] <GetStringByKey> key = %s, value = %s", ObjKey, RetString);

    return 0;
}

int AppLibNetJsonUtility_CreateObject(APPLIB_JSON_OBJECT **JsonObject)
{
    AMP_JSON_OBJECT_s *pstJsonObject = NULL;

    if (!JsonObject) {
        JSON_ERR("[AppLib - NetJsonUtility] <CreateObject> invalid param (JsonObject = 0x%08x)", (UINT32)JsonObject);
        return -1;
    }

    *JsonObject = NULL;
    pstJsonObject = AmpJson_CreateObject();
    if (!pstJsonObject) {
        JSON_ERR("[AppLib - NetJsonUtility] <CreateObject> AmpJson_CreateObject() fail");
        return -1;
    }

    *JsonObject = (APPLIB_JSON_OBJECT*)pstJsonObject;

    return 0;
}

int AppLibNetJsonUtility_FreeJsonObject(APPLIB_JSON_OBJECT *JsonObject)
{
    AMP_JSON_OBJECT_s *pstJsonObject = (AMP_JSON_OBJECT_s *)JsonObject;
    int ReturnValue = 0;

    if (!pstJsonObject) {
        JSON_ERR("[AppLib - NetJsonUtility] <FreeJsonObject> invalid param");
        return -1;
    }

    ReturnValue = AmpJson_FreeJsonObject(pstJsonObject);
    if (ReturnValue != 0) {
         JSON_ERR("[AppLib - NetJsonUtility] <FreeJsonObject> AmpJson_FreeJsonObject() fail. (Err: %d)",ReturnValue);
         return -1;
    }

    return 0;
}

int AppLibNetJsonUtility_AddIntegerObject(APPLIB_JSON_OBJECT *JsonObject, char *Key, int Value)
{
    AMP_JSON_OBJECT_s *pstJsonObject = (AMP_JSON_OBJECT_s *)JsonObject;

    if ((!pstJsonObject) || (!Key)) {
        JSON_ERR("[AppLib - NetJsonUtility] <AddIntegerObject> invalid param");
        return -1;
    }

    AmpJson_AddObject(pstJsonObject, Key, AmpJson_CreateIntegerObject(Value));

    return 0;
}

int AppLibNetJsonUtility_AddStringObject(APPLIB_JSON_OBJECT *JsonObject, char *Key, char *Value)
{
    AMP_JSON_OBJECT_s *pstJsonObject = (AMP_JSON_OBJECT_s *)JsonObject;

    if ((!pstJsonObject) || (!Key) || (!Value)) {
        JSON_ERR("[AppLib - NetJsonUtility] <AddStringObject> invalid param");
        return -1;
    }

    AmpJson_AddObject(pstJsonObject, Key, AmpJson_CreateStringObject(Value));

    return 0;
}

int AppLibNetJsonUtility_AddObject(APPLIB_JSON_OBJECT *JsonObject, char *Key, APPLIB_JSON_OBJECT *JsonArrayObject)
{
    AMP_JSON_OBJECT_s *pstJsonObject = (AMP_JSON_OBJECT_s *)JsonObject;
    AMP_JSON_OBJECT_s *pstJsonArrayObject = (AMP_JSON_OBJECT_s *)JsonArrayObject;

    if ((!pstJsonObject) || (!Key) || (!pstJsonArrayObject)) {
        JSON_ERR("[AppLib - NetJsonUtility] <AddObject> invalid param");
        return -1;
    }

    AmpJson_AddObject(pstJsonObject, Key, JsonArrayObject);

    return 0;
}

int AppLibNetJsonUtility_CreateArrayObject(APPLIB_JSON_OBJECT **JsonObject)
{
    AMP_JSON_OBJECT_s *pstJsonObject = NULL;

    if (!JsonObject) {
        JSON_ERR("[AppLib - NetJsonUtility] <CreateArrayObject> invalid param (JsonObject = 0x%08x)", (UINT32)JsonObject);
        return -1;
    }

    *JsonObject = NULL;
    pstJsonObject = AmpJson_CreateArrayObject();
    if (!pstJsonObject) {
        JSON_ERR("[AppLib - NetJsonUtility] <CreateArrayObject> AmpJson_CreateArrayObject() fail");
        return -1;
    }

    *JsonObject = (APPLIB_JSON_OBJECT*)pstJsonObject;
    return 0;
}

int AppLibNetJsonUtility_FreeJsonArrayObject(APPLIB_JSON_OBJECT *JsonObject)
{
    AMP_JSON_OBJECT_s *pstJsonObject = (AMP_JSON_OBJECT_s *)JsonObject;
    int ReturnValue = 0;

    if (!pstJsonObject) {
        JSON_ERR("[AppLib - NetJsonUtility] <FreeJsonArrayObject> invalid param");
        return -1;
    }

    ReturnValue = AmpJson_FreeJsonObject(pstJsonObject->ObjValue.Array);
    if (ReturnValue != 0) {
         JSON_ERR("[AppLib - NetJsonUtility] <FreeJsonArrayObject> AmpJson_FreeJsonObject() fail. (Err: %d)",ReturnValue);
         return -1;
    }

    return 0;
}

int AppLibNetJsonUtility_AddObjectToArray(APPLIB_JSON_OBJECT *JsonObject, char *String)
{
    AMP_JSON_OBJECT_s *pstJsonObject = (AMP_JSON_OBJECT_s *)JsonObject;
    int ReturnValue = 0;

    if (!pstJsonObject || (!String)) {
        JSON_ERR("[AppLib - NetJsonUtility] <AddObjectToArray> invalid param");
        return -1;
    }

    ReturnValue = AmpJson_AddObjectToArray(pstJsonObject, AmpJson_CreateStringObject(String));
    if (ReturnValue != 0) {
        JSON_ERR("[AppLib - NetJsonUtility] <AddObjectToArray> AmpJson_AddObjectToArray() fail");
        return -1;
    }

    return 0;
}

int AppLibNetJsonUtility_JsonObjectToJsonString(APPLIB_JSON_OBJECT *JsonObject, APPLIB_JSON_STRING **JsonString)
{
    AMP_JSON_OBJECT_s *pstJsonObject = (AMP_JSON_OBJECT_s *)JsonObject;
    AMP_JSON_STRING_s *pstJsonString = NULL;


    *JsonString = NULL;
    pstJsonString = AmpJson_JsonObjectToJsonString(pstJsonObject);
    if (!pstJsonString) {
        JSON_ERR("[AppLib - NetJsonUtility] <JsonObjectToJsonString> AmpJson_JsonObjectToJsonString() fail");
        return -1;
    }

    *JsonString = (APPLIB_JSON_STRING *)pstJsonString;

    return 0;
}

int AppLibNetJsonUtility_GetString(APPLIB_JSON_STRING *JsonString, char **String)
{
    AMP_JSON_STRING_s *pstJsonString = (AMP_JSON_STRING_s *)JsonString;

    if (!pstJsonString || (!String)) {
        JSON_ERR("[AppLib - NetJsonUtility] <GetString> invalid param");
        return -1;
    }

    *String = pstJsonString->JsonString;

    return 0;
}

int AppLibNetJsonUtility_FreeJsonString(APPLIB_JSON_STRING *JsonString)
{
    AMP_JSON_STRING_s *pstJsonString = (AMP_JSON_STRING_s *)JsonString;
    int ReturnValue = 0;

    if (!pstJsonString) {
        JSON_ERR("[AppLib - NetJsonUtility] <FreeJsonString> invalid param");
        return -1;
    }

    ReturnValue = AmpJson_FreeJsonString(pstJsonString);
    if (ReturnValue != 0) {
         JSON_ERR("[AppLib - NetJsonUtility] <FreeJsonString> AmpJson_FreeJsonString() fail. (Err: %d)",ReturnValue);
         return -1;
    }

    return 0;
}


