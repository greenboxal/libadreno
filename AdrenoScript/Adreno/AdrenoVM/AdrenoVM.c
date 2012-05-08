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
	AdrenoValue value, value2, value3;
	AdrenoValue *rvalue, *rvalue2, *rvalue3;
	AdrenoOpcodes op;
	AdrenoOpcodePrefixes prefix;
	AdrenoOpcodeData *opcode;
	unsigned int i, i4, opSize;

	if (!ctx->LoadedScript)
		return;

	if (!ctx->CurrentFunction)
		return;

	vm->State == ST_RUN;
	while (vm->State == ST_RUN)
	{
		opSize = 1;
		op = (AdrenoOpcodes)(ctx->CurrentFunction->Bytecode[ctx->InstructionPointer] & OP_MASK);
		prefix = (AdrenoOpcodePrefixes)(ctx->CurrentFunction->Bytecode[ctx->InstructionPointer] & P_MASK);
		opcode = (AdrenoOpcodeData *)(&ctx->CurrentFunction->Bytecode[ctx->InstructionPointer + 1]);

		switch (op)
		{
		case OP_NOP:
			break;

			// Stack
		case OP_POP:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value))
				{
					vm->State = ST_END;
					break;
				}

				AdrenoValue_Dereference(&value);
			}
			break;

			// Locals
		case OP_STLOC_0:
		case OP_STLOC_1:
		case OP_STLOC_2:
		case OP_STLOC_3:
		case OP_STLOC_S:
			{
				int index;

				if (op >= OP_STLOC_0 && op <= OP_STLOC_3)
				{
					index = op - OP_STLOC_0;
				}
				else
				{
					opSize += 4;
					index = opcode->Value.I4;
				}

				if (!AdrenoStack_Pop(&ctx->Stack, &ctx->Locals[index]))
				{
					vm->State = ST_END;
					break;
				}
			}
			break;
		case OP_LDLOC_0:
		case OP_LDLOC_1:
		case OP_LDLOC_2:
		case OP_LDLOC_3:
		case OP_LDLOC_S:
			{
				int index;

				if (op >= OP_STLOC_0 && op <= OP_STLOC_3)
				{
					index = op - OP_STLOC_0;
				}
				else
				{
					opSize += 4;
					index = opcode->Value.I4;
				}

				if (!AdrenoStack_Push(&ctx->Stack, &ctx->Locals[index], ADRENOSTACK_CAN_EXPAND))
				{
					vm->State = ST_END;
					break;
				}
			}
			break;

			// Constansts
		case OP_LDNULL:
			{
				value.Type = AT_NULL;
				value.GCFlags = GC_NONE;
				value.ReferenceCounter = 1;
				value.Value.I4 = 0;

				if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
				{
					vm->State = ST_END;
					break;
				}
			}
			break;
		case OP_LDC_I4:
		case OP_LDC_I4_M1:
		case OP_LDC_I4_0:
		case OP_LDC_I4_1:
			{
				if (op >= OP_LDC_I4_M1 && op <= OP_LDC_I4_1)
				{
					i4 = op - OP_LDC_I4_0;
				}
				else
				{
					opSize += 4;
					i4 = opcode->Value.I4;
				}

				value.Type = AT_INTEGER;
				value.GCFlags = GC_NONE;
				value.ReferenceCounter = 1;
				value.Value.I4 = i4;

				if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
				{
					vm->State = ST_END;
					break;
				}
			}
			break;
		case OP_LDSTR:
			{
				opSize += 4;

				value.Type = AT_STRING;
				value.GCFlags = GC_NONE;
				value.ReferenceCounter = 1;
				value.Value.String.Value = (wchar_t *)ctx->LoadedScript->Strings.NodeHeap[opcode->Value.I4].Value.Value;
				value.Value.String.Size = wcslen(value.Value.String.Value);

				if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
				{
					vm->State = ST_END;
					break;
				}
			}
			break;

			// Mathematical Operations
		case OP_ADD:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value) && !AdrenoStack_Pop(&ctx->Stack, &value2))
				{
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);
				rvalue2 = AdrenoValue_GetValue(&value2);

				if (!rvalue || !rvalue2)
				{
					vm->State = ST_END;
					break;
				}

				value3.GCFlags = GC_NONE;
				value3.ReferenceCounter = 1;

				if (value.Type == AT_INTEGER && value2.Type == AT_INTEGER)
				{
					value3.Type = AT_INTEGER;
					value3.Value.I4 = value.Value.I4 + value2.Value.I4;
				}

				AdrenoValue_Dereference(&value);
				AdrenoValue_Dereference(&value2);
			}
			break;
		case OP_SUB:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value) && !AdrenoStack_Pop(&ctx->Stack, &value2))
				{
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);
				rvalue2 = AdrenoValue_GetValue(&value2);

				if (!rvalue || !rvalue2)
				{
					vm->State = ST_END;
					break;
				}

				value3.GCFlags = GC_NONE;
				value3.ReferenceCounter = 1;

				if (value.Type == AT_INTEGER && value2.Type == AT_INTEGER)
				{
					value3.Type = AT_INTEGER;
					value3.Value.I4 = value.Value.I4 - value2.Value.I4;
				}

				AdrenoValue_Dereference(&value);
				AdrenoValue_Dereference(&value2);
			}
			break;
		case OP_MUL:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value) && !AdrenoStack_Pop(&ctx->Stack, &value2))
				{
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);
				rvalue2 = AdrenoValue_GetValue(&value2);

				if (!rvalue || !rvalue2)
				{
					vm->State = ST_END;
					break;
				}

				value3.GCFlags = GC_NONE;
				value3.ReferenceCounter = 1;

				if (value.Type == AT_INTEGER && value2.Type == AT_INTEGER)
				{
					value3.Type = AT_INTEGER;
					value3.Value.I4 = value.Value.I4 * value2.Value.I4;
				}

				AdrenoValue_Dereference(&value);
				AdrenoValue_Dereference(&value2);
			}
			break;
		case OP_DIV:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value) && !AdrenoStack_Pop(&ctx->Stack, &value2))
				{
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);
				rvalue2 = AdrenoValue_GetValue(&value2);

				if (!rvalue || !rvalue2)
				{
					vm->State = ST_END;
					break;
				}

				value3.GCFlags = GC_NONE;
				value3.ReferenceCounter = 1;

				if (value.Type == AT_INTEGER && value2.Type == AT_INTEGER)
				{
					value3.Type = AT_INTEGER;
					value3.Value.I4 = value.Value.I4 / value2.Value.I4;
				}

				AdrenoValue_Dereference(&value);
				AdrenoValue_Dereference(&value2);
			}
			break;
		case OP_REM:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value) && !AdrenoStack_Pop(&ctx->Stack, &value2))
				{
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);
				rvalue2 = AdrenoValue_GetValue(&value2);

				if (!rvalue || !rvalue2)
				{
					vm->State = ST_END;
					break;
				}

				value3.GCFlags = GC_NONE;
				value3.ReferenceCounter = 1;

				if (value.Type == AT_INTEGER && value2.Type == AT_INTEGER)
				{
					value3.Type = AT_INTEGER;
					value3.Value.I4 = value.Value.I4 % value2.Value.I4;
				}

				AdrenoValue_Dereference(&value);
				AdrenoValue_Dereference(&value2);
			}
			break;

			// Bitwise Operations
		case OP_OR:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value) && !AdrenoStack_Pop(&ctx->Stack, &value2))
				{
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);
				rvalue2 = AdrenoValue_GetValue(&value2);

				if (!rvalue || !rvalue2)
				{
					vm->State = ST_END;
					break;
				}

				value3.GCFlags = GC_NONE;
				value3.ReferenceCounter = 1;

				if (value.Type == AT_INTEGER && value2.Type == AT_INTEGER)
				{
					value3.Type = AT_INTEGER;
					value3.Value.I4 = value.Value.I4 | value2.Value.I4;
				}

				AdrenoValue_Dereference(&value);
				AdrenoValue_Dereference(&value2);
			}
			break;
		case OP_AND:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value) && !AdrenoStack_Pop(&ctx->Stack, &value2))
				{
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);
				rvalue2 = AdrenoValue_GetValue(&value2);

				if (!rvalue || !rvalue2)
				{
					vm->State = ST_END;
					break;
				}

				value3.GCFlags = GC_NONE;
				value3.ReferenceCounter = 1;

				if (value.Type == AT_INTEGER && value2.Type == AT_INTEGER)
				{
					value3.Type = AT_INTEGER;
					value3.Value.I4 = value.Value.I4 & value2.Value.I4;
				}

				AdrenoValue_Dereference(&value);
				AdrenoValue_Dereference(&value2);
			}
			break;
		case OP_XOR:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value) && !AdrenoStack_Pop(&ctx->Stack, &value2))
				{
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);
				rvalue2 = AdrenoValue_GetValue(&value2);

				if (!rvalue || !rvalue2)
				{
					vm->State = ST_END;
					break;
				}

				value3.GCFlags = GC_NONE;
				value3.ReferenceCounter = 1;

				if (value.Type == AT_INTEGER && value2.Type == AT_INTEGER)
				{
					value3.Type = AT_INTEGER;
					value3.Value.I4 = value.Value.I4 ^ value2.Value.I4;
				}

				AdrenoValue_Dereference(&value);
				AdrenoValue_Dereference(&value2);
			}
			break;

		default:
			{
				vm->State = ST_END;
			}
			break;
		}

		if (opSize > 0)
			ctx->InstructionPointer += opSize;

		if (ctx->InstructionPointer >= ctx->CurrentFunction->BytecodeSize)
		{
			vm->State = ST_END;
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

