#ifndef ADRENOVM_H
#define ADRENOVM_H

#include "../AdrenoHelpers/AdrenoHashtable.h"

typedef struct adrenovm AdrenoVM;
typedef struct adrenocontext AdrenoContext;
typedef struct adrenoscript AdrenoScript;
typedef struct adrenoop AdrenoOpcodeData;
typedef struct adrenofunction AdrenoFunction;
typedef struct adrenostring AdrenoString;
typedef struct adrenoarray AdrenoArray;
typedef struct adrenoretvalue AdrenoReturnInfo;
typedef struct adrenovalue AdrenoValue;
typedef struct adrenostack AdrenoStack;

#include "../AdrenoConfig.h"
#include "../AdrenoMemory.h"
#include "AdrenoScript.h"
#include "AdrenoValue.h"
#include "AdrenoStack.h"

typedef enum
{
    P_NONE = 0x00,
    P_REFERENCE = 0x80,
    P_MASK = 0x80,
} AdrenoOpcodePrefixes;

typedef enum
{
    OP_NOP,

    // Stack
    OP_POP,

    // Locals
    OP_STLOC_0,
    OP_STLOC_1,
    OP_STLOC_2,
    OP_STLOC_3,
    OP_STLOC_S,
    OP_LDLOC_0,
    OP_LDLOC_1,
    OP_LDLOC_2,
    OP_LDLOC_3,
    OP_LDLOC_S,

    // Arguments
    OP_LDARG_0,
    OP_LDARG_1,
    OP_LDARG_2,
    OP_LDARG_3,
    OP_LDARG_S,

    // Constants
    OP_LDNULL,
    OP_LDC_I4,
    OP_LDC_I4_M1,
    OP_LDC_I4_0,
    OP_LDC_I4_1,
    OP_LDSTR,

    // Arrays
    OP_NEWARR,
    OP_STELEM,
    OP_LDELEM,
	OP_RMELEM,
    OP_LDLEN,

    // Mathematical Operations
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_REM,
    OP_NEG,

    // Bitwise Operations
    OP_OR,
    OP_AND,
    OP_XOR,
    OP_NOT,
    OP_SHL,
    OP_SHR,

    // Logical Operations
    OP_LOR,
    OP_LAND,
    OP_LNOT,
    OP_EQ,
    OP_NE,
    OP_GT,
    OP_GE,
    OP_LT,
    OP_LE,

    // Flow Control
    OP_JUMP,
    OP_BRTRUE,
    OP_BRFALSE,
    OP_SWITCH,

    // Functions
    OP_LDFUNC,
    OP_CALL,
    OP_ENTER,
    OP_RET,

    OP_MASK = 0x7F,
} AdrenoOpcodes;

typedef enum
{
    ST_IDLE,
    ST_RUN,
    ST_END,
} AdrenoVMState;

typedef enum
{
	ERR_NONE,
	ERR_DIVISON_BY_0,
	ERR_BAD_OP,
	ERR_STACK_OVERFLOW,
	ERR_STACK_UNDERFLOW,
	ERR_NULL_REFERENCE,
	ERR_OUT_OF_BOUNDS,
	ERR_FOREING_CAST,
	ERR_INVALID_OPERAND,
} AdrenoVMError;

struct adrenoop
{
	union
	{
		unsigned char I1;
		unsigned short I2;
		unsigned int I4;
	} Value;
};

struct adrenocontext
{
	AdrenoStack Stack;

	AdrenoScript *LoadedScript;
	AdrenoFunction *CurrentFunction;
	unsigned int InstructionPointer;

	AdrenoValue *Locals;
};

struct adrenovm
{
	AdrenoVMState State;
	AdrenoVMError Error;

	AdrenoHashtable GlobalFunctions;
};

#ifdef __cplusplus
extern "C"
{
#endif

	extern void AdrenoVM_Initialize(AdrenoVM *vm);
	extern unsigned int AdrenoVM_AttachScript(AdrenoVM *vm, AdrenoScript *script);
	extern void AdrenoVM_Run(AdrenoVM *vm, AdrenoContext *ctx);
	extern void AdrenoVM_Free(AdrenoVM *vm);

	extern void AdrenoContext_Initialize(AdrenoContext *ctx);
	extern void AdrenoContext_AttachScript(AdrenoContext *ctx, AdrenoScript *script);
	extern void AdrenoContext_SetFunctionByName(AdrenoContext *ctx, wchar_t *name);
	extern void AdrenoContext_SetFunction(AdrenoContext *ctx, AdrenoFunction *func);
	extern void AdrenoContext_Free(AdrenoContext *ctx);

#ifdef __cplusplus
}
#endif

#define ADRENOVALUE_IS_NULL_VALUE(value) (value.Type == AT_NULL)
#define ADRENOVALUE_IS_NULL_VALUE_PTR(value) (value == NULL || value->Type == AT_NULL)

#endif
