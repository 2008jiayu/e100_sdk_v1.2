/*-------------------------------------------------------------------------------------------------------------------*\
 *  @FileName       :: AmbaTUNE_Parser.h
 *
 *  @Copyright      :: Copyright (C) 2012 Ambarella Corporation. All rights reserved.
 *
 *                     No part of this file may be reproduced, stored in a retrieval system,
 *                     or transmitted, in any form, or by any means, electronic, mechanical, photocopying,
 *                     recording, or otherwise, without the prior consent of Ambarella Corporation.
 *
 *  @Description    :: Definitions & Constants for Ambarella TUNE String Parser Utility
\*-------------------------------------------------------------------------------------------------------------------*/
#include "AmbaDataType.h"
#define MAX_NAME_LENS       (128)
#define MAX_LINE_CHARS (35000) // (27000)
typedef enum {
    EXT_PATH = 0,
    STRING,
    U_8,
    S_8,
    U_16,
    S_16,
    U_32,
    S_32,
    TYPE_MAX,
} Param_Value_Type_e;

typedef enum {
    ITUNER_ENC = 0,
    ITUNER_DEC,
} OpMode_e;

typedef struct _PARAM_s_ {
    int     Index;
    char    Name[MAX_NAME_LENS];
    Param_Value_Type_e ValType;
    UINT32  ValCount;
    char*   ValStr;
} PARAM_s;
typedef INT64 (proc_t)(int, PARAM_s*);

typedef struct _REG_s_ {
    int      Index;
    char     Name[MAX_NAME_LENS];
    int      RegNum;        //!< Reserved
    int      RegEntrySize; //!< Reserved
    int      ParamCount;
    INT64    Valid;
    PARAM_s* ParamList;
    proc_t*  Process;
} REG_s;

typedef struct {
    REG_s** RegList;
    int RegCount;
    char* LineBuf;
    int BufSize;
}TUNE_Parser_Object_t;



void TUNE_Parser_Enc_Proc(PARAM_s* pParam, void* pField);
void TUNE_Parser_Dec_Proc(PARAM_s* pParam, void* pField);
void TUNE_Parser_Param_Proc(PARAM_s *pParam, void* pField);
void TUNE_Parser_Set_Opmode(OpMode_e OpMode);
OpMode_e TUNE_Parser_Get_Opmode(void);
int TUNE_Parser_Create(TUNE_Parser_Object_t** Object);
int TUNE_Parser_Destory(TUNE_Parser_Object_t* Object);
int TUNE_Parser_Add_Reg(REG_s *pReg, TUNE_Parser_Object_t* Object);
int TUNE_Parser_Set_Reglist_Valid(INT64 Valid, TUNE_Parser_Object_t* Object);
int TUNER_Parser_Set_Reg_Valid(const char* Name, INT64 Valid, TUNE_Parser_Object_t* Object);
REG_s* TUNE_Parser_Lookup_Reg(const char* Name, TUNE_Parser_Object_t* Object);
PARAM_s* TUNE_Parser_Lookup_Param(const char* Name, REG_s* Reg);
int TUNE_Parser_Get_LineBuf(TUNE_Parser_Object_t* Object, char** Line, int* Size);
int TUNE_Parser_Parse_Line(TUNE_Parser_Object_t* Object);
REG_s* TUNE_Parser_Get_Reg( int RegIdx, TUNE_Parser_Object_t* Object);
PARAM_s* TUNE_Parser_Get_Param(REG_s* Reg, int ParamIdx);
int TUNE_Parser_Generate_Line(int RegIdx, int ParamIdx, TUNE_Parser_Object_t* Object);

