#include "AdrenoVM.h"

void AdrenoVM_Initialize(AdrenoVM *vm)
{
	AdrenoHashtable_Init(&vm->GlobalFunctions, AdrenoHashtable_Hash_Fnv, (AdrenoHashtable_LenFunction)wcslen);
	AdrenoHashtable_Init(&vm->LoadedScripts, NULL, NULL);

	vm->State = ST_IDLE;
}

unsigned int AdrenoVM_AttachScript(AdrenoVM *vm, AdrenoScript *script)
{
	unsigned int index = vm->LoadedScripts.NodeCount;

	AdrenoHashtable_Set(&vm->LoadedScripts, (void *)index, script);

	return index;
}

void AdrenoVM_Run(AdrenoVM *vm, AdrenoContext *ctx)
{
	AdrenoOpcodes op;
	AdrenoOpcodePrefixes prefix;
	AdrenoOpcodeData *opcode;
	unsigned int i, i4;

	if (!ctx->LoadedScript)
		return;

	if (!ctx->CurrentFunction)
		return;

	vm->State == ST_RUN;
	while (vm->State == ST_RUN)
	{
		op = (AdrenoOpcodes)(ctx->CurrentFunction->Bytecode[ctx->InstructionPointer] & OP_MASK);
		prefix = (AdrenoOpcodePrefixes)(ctx->CurrentFunction->Bytecode[ctx->InstructionPointer] & P_MASK);
		opcode = (AdrenoOpcodeData *)(&ctx->CurrentFunction->Bytecode[ctx->InstructionPointer + 1]);

		switch (op)
		{
		default:
			{
				vm->State = ST_END;
			}
			break;
		}
	}
}

void AdrenoVM_Free(AdrenoVM *vm)
{
	AdrenoHashtable_Destroy(&vm->GlobalFunctions);
	AdrenoHashtable_Destroy(&vm->LoadedScripts);
}

void AdrenoContext_Initialize(AdrenoContext *ctx)
{
	ctx->LoadedScript = NULL;
	ctx->CurrentFunction = NULL;
	ctx->Locals = NULL;
	ctx->InstructionPointer = 0;

	AdrenoStack_Initialize(&ctx->Stack, ADRENOSTACK_DEFAULT_STACK);
}

void AdrenoContext_AttachScript(AdrenoContext *ctx, AdrenoScript *script)
{
	ctx->LoadedScript = script;
}

void AdrenoContext_SetFunctionByName(AdrenoContext *ctx, wchar_t *name)
{
	AdrenoFunction *func;

	if (!ctx->LoadedScript)
		return;

	if (!AdrenoHashtable_Get(&ctx->LoadedScript->Functions, name, (void **)&func))
		return;

	AdrenoContext_SetFunction(ctx, func);
}

void AdrenoContext_SetFunction(AdrenoContext *ctx, AdrenoFunction *func)
{
	ctx->CurrentFunction = func;
}

void AdrenoContext_Free(AdrenoContext *ctx)
{
	AdrenoStack_Clear(&ctx->Stack);
}

