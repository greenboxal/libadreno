#include "AdrenoVM.h"

void AdrenoVM_Initialize(AdrenoVM *vm)
{
	AdrenoHashtable_Init(&vm->GlobalFunctions, AdrenoHashtable_Hash_Fnv, (AdrenoHashtable_LenFunction)wcslen);

	vm->GlobalFunctions.ExpansionFactor = 2;

	vm->State = ST_IDLE;
}

AdrenoFunction *AdrenoVM_GetFunction(AdrenoVM *vm, AdrenoContext *ctx, AdrenoValue *value)
{
	AdrenoFunction *function = NULL;

	if (value->Type == AT_FUNCTION)
	{
		function = value->Value.Function;
	}
	else if (value->Type == AT_STRING)
	{
		if (!AdrenoHashtable_Get(&ctx->LoadedScript->Functions, value->Value.String->Value, (void **)&function))
			if (!AdrenoHashtable_Get(&vm->GlobalFunctions, value->Value.String->Value, (void **)&function))
				function = NULL;
	}

	return function;
}

#define ADRENOVM_MATHOP_SINGLE_CONTRACT \
				if (!AdrenoStack_Pop(&ctx->Stack, &value)) \
				{ \
					vm->Error = ERR_STACK_UNDERFLOW; \
					vm->State = ST_END; \
					break; \
				} \
				 \
				rvalue = AdrenoValue_GetValue(&value); \
				 \
				if (ADRENOVALUE_IS_NULL_VALUE_PTR(rvalue)) \
				{ \
					vm->Error = ERR_NULL_REFERENCE; \
					vm->State = ST_END; \
					break; \
				} \
				 \
				value3.GCFlags = GC_NONE; \
				value3.ReferenceCounter = 0;

#define ADRENOVM_MATHOP_SINGLE_START() \
					if (0) ;

#define ADRENOVM_MATHOP_SINGLE_SIMPLE_OP(typeName, fieldName, oper) \
					else if (rvalue->Type == typeName) \
					{ \
						value3.Type = typeName; \
						value3.Value.fieldName = oper rvalue->Value.fieldName; \
					}

#define ADRENOVM_MATHOP_SINGLE_FINISH() \
					else \
					{ \
						vm->Error = ERR_INVALID_OPERAND; \
						vm->State = ST_END; \
						break; \
					}

#define ADRENOVM_MATHOP_SINGLE_ENDCONTRACT \
				AdrenoValue_Dereference(&value); \
				 \
				if (!AdrenoStack_Push(&ctx->Stack, &value3, ADRENOSTACK_CAN_EXPAND)) \
				{ \
					vm->Error = ERR_STACK_OVERFLOW; \
					vm->State = ST_END; \
					break; \
				}

#define ADRENOVM_MATHOP_DUAL_CONTRACT \
				if (!AdrenoStack_Pop(&ctx->Stack, &value2) || !AdrenoStack_Pop(&ctx->Stack, &value)) \
				{ \
					vm->Error = ERR_STACK_UNDERFLOW; \
					vm->State = ST_END; \
					break; \
				} \
				 \
				rvalue = AdrenoValue_GetValue(&value); \
				rvalue2 = AdrenoValue_GetValue(&value2); \
				 \
				if (ADRENOVALUE_IS_NULL_VALUE_PTR(rvalue) || ADRENOVALUE_IS_NULL_VALUE_PTR(rvalue2)) \
				{ \
					vm->Error = ERR_NULL_REFERENCE; \
					vm->State = ST_END; \
					break; \
				} \
				 \
				value3.GCFlags = GC_NONE; \
				value3.ReferenceCounter = 0;

#define ADRENOVM_MATHOP_DUAL_START() \
					if (0) ;

#define ADRENOVM_MATHOP_DUAL_SIMPLE_OP(typeName, fieldName, oper) \
					else if (rvalue->Type == typeName && rvalue2->Type == typeName) \
					{ \
						value3.Type = typeName; \
						value3.Value.fieldName = rvalue->Value.fieldName oper rvalue2->Value.fieldName; \
					}

#define ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE() \
					else if (rvalue->Type != rvalue2->Type) \
					{ \
						vm->Error = ERR_FOREING_CAST; \
						vm->State = ST_END; \
						break; \
					}

#define ADRENOVM_MATHOP_DUAL_SIMPLE_OP_0CHECK(typeName, fieldName, oper) \
					else if (rvalue->Type == typeName && rvalue2->Type == typeName) \
					{ \
						if (rvalue2->Value.I4 == 0) \
						{ \
							vm->Error = ERR_DIVISON_BY_0; \
							vm->State = ST_END; \
							break; \
						} \
						 \
						value3.Type = typeName; \
						value3.Value.I4 = rvalue->Value.fieldName oper rvalue2->Value.fieldName; \
					}

#define ADRENOVM_MATHOP_DUAL_CHECKTYPE(type) \
					else if (rvalue->Type == type && rvalue2->Type == type)

#define ADRENOVM_MATHOP_DUAL_CHECKTYPE2(type1, type2) \
					else if (rvalue->Type == type1 && rvalue2->Type == type2)

#define ADRENOVM_MATHOP_DUAL_STRINGCAT() \
						value3.Type = AT_STRING; \
						value3.GCFlags = GC_COLLECT; \
						value3.Value.String = (AdrenoString *)AdrenoAlloc(sizeof(AdrenoString)); \
						value3.Value.String->Size = rvalue->Value.String->Size + rvalue2->Value.String->Size; \
						value3.Value.String->Value = (wchar_t *)AdrenoAlloc(value3.Value.String->Size * sizeof(wchar_t) + 2); \
						value3.Value.String->Flags = SF_FREE; \
						 \
						memcpy(&value3.Value.String->Value[0], rvalue->Value.String->Value, rvalue->Value.String->Size * sizeof(wchar_t)); \
						memcpy(&value3.Value.String->Value[rvalue->Value.String->Size], rvalue2->Value.String->Value, rvalue2->Value.String->Size * sizeof(wchar_t)); \
						value3.Value.String->Value[value3.Value.String->Size] = 0;

#define ADRENOVM_MATHOP_DUAL_FINISH() \
					else \
					{ \
						vm->Error = ERR_INVALID_OPERAND; \
						vm->State = ST_END; \
						break; \
					}

#define ADRENOVM_MATHOP_DUAL_ENDCONTRACT \
				AdrenoValue_Dereference(&value); \
				AdrenoValue_Dereference(&value2); \
				 \
				if (!AdrenoStack_Push(&ctx->Stack, &value3, ADRENOSTACK_CAN_EXPAND)) \
				{ \
					vm->Error = ERR_STACK_OVERFLOW; \
					vm->State = ST_END; \
					break; \
				}


void AdrenoVM_Run(AdrenoVM *vm, AdrenoContext *ctx)
{
	AdrenoValue value, value2, value3;
	AdrenoValue *rvalue, *rvalue2;
	AdrenoOpcodes op;
	AdrenoOpcodePrefixes prefix;
	AdrenoOpcodeData *opcode;
	unsigned int i, i4, opSize;

	if (!ctx->LoadedScript)
		return;

	if (!ctx->CurrentFunction)
		return;

	vm->State = ST_RUN;
	vm->Error = ERR_NONE;
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
					vm->Error = ERR_STACK_UNDERFLOW;
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
					vm->Error = ERR_STACK_UNDERFLOW;
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

				if (op >= OP_LDLOC_0 && op <= OP_LDLOC_3)
				{
					index = op - OP_LDLOC_0;
				}
				else
				{
					opSize += 4;
					index = opcode->Value.I4;
				}

				if (!AdrenoStack_Push(&ctx->Stack, &ctx->Locals[index], ADRENOSTACK_CAN_EXPAND))
				{
					vm->Error = ERR_STACK_OVERFLOW;
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
				value.ReferenceCounter = 0;
				value.Value.I4 = 0;

				if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
				{
					vm->Error = ERR_STACK_OVERFLOW;
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
				value.ReferenceCounter = 0;
				value.Value.I4 = i4;

				if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
				{
					vm->Error = ERR_STACK_OVERFLOW;
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
				value.ReferenceCounter = 0;
				value.Value.String = (AdrenoString *)ctx->LoadedScript->Strings.NodeHeap[opcode->Value.I4].Value.Value;

				if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
				{
					vm->Error = ERR_STACK_OVERFLOW;
					vm->State = ST_END;
					break;
				}
			}
			break;

			// Mathematical Operations
		case OP_ADD:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, +)
					ADRENOVM_MATHOP_DUAL_CHECKTYPE(AT_STRING)
					{
						ADRENOVM_MATHOP_DUAL_STRINGCAT();
					}
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_SUB:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, -)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_MUL:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, *)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_DIV:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP_0CHECK(AT_INTEGER, I4, /)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_REM:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP_0CHECK(AT_INTEGER, I4, %)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_NEG:
			{
				ADRENOVM_MATHOP_SINGLE_CONTRACT
				{
					ADRENOVM_MATHOP_SINGLE_START()
					ADRENOVM_MATHOP_SINGLE_SIMPLE_OP(AT_INTEGER, I4, -(int))
					ADRENOVM_MATHOP_SINGLE_FINISH()
				}
				ADRENOVM_MATHOP_SINGLE_ENDCONTRACT
			}
			break;

			// Bitwise Operations
		case OP_OR:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, |)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_AND:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, &)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_XOR:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, ^)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_NOT:
			{
				ADRENOVM_MATHOP_SINGLE_CONTRACT
				{
					ADRENOVM_MATHOP_SINGLE_START()
					ADRENOVM_MATHOP_SINGLE_SIMPLE_OP(AT_INTEGER, I4, ~)
					ADRENOVM_MATHOP_SINGLE_FINISH()
				}
				ADRENOVM_MATHOP_SINGLE_ENDCONTRACT
			}
			break;
		case OP_SHL:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, <<)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_SHR:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, >>)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;

			// Logical Operations
		case OP_LOR:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, ||)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_LAND:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, &&)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_LNOT:
			{
				ADRENOVM_MATHOP_SINGLE_CONTRACT
				{
					ADRENOVM_MATHOP_SINGLE_START()
					ADRENOVM_MATHOP_SINGLE_SIMPLE_OP(AT_INTEGER, I4, !)
					ADRENOVM_MATHOP_SINGLE_FINISH()
				}
				ADRENOVM_MATHOP_SINGLE_ENDCONTRACT
			}
			break;
		case OP_EQ:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, ==)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_NE:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, !=)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_GT:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, >)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_GE:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, >=)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_LT:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, <)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;
		case OP_LE:
			{
				ADRENOVM_MATHOP_DUAL_CONTRACT
				{
					ADRENOVM_MATHOP_DUAL_START()
					ADRENOVM_MATHOP_DUAL_CHECKDIFTYPE()
					ADRENOVM_MATHOP_DUAL_SIMPLE_OP(AT_INTEGER, I4, <=)
					ADRENOVM_MATHOP_DUAL_FINISH()
				}
				ADRENOVM_MATHOP_DUAL_ENDCONTRACT
			}
			break;

			// Flow Control
		case OP_JUMP:
			{
				opSize += 4 + opcode->Value.I4;
			}
			break;
		case OP_BRTRUE:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value))
				{
					vm->Error = ERR_STACK_UNDERFLOW;
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);
				
				if (ADRENOVALUE_IS_NULL_VALUE_PTR(rvalue))
				{
					vm->Error = ERR_NULL_REFERENCE;
					vm->State = ST_END;
					break;
				}

				opSize += 4;
					
				if (rvalue->Value.I4)
					opSize += opcode->Value.I4;

				AdrenoValue_Dereference(&value);
			}
			break;
		case OP_BRFALSE:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value))
				{
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);

				if (!rvalue)
				{
					vm->State = ST_END;
					break;
				}

				opSize += 4;
					
				if (!rvalue->Value.I4)
					opSize += opcode->Value.I4;

				AdrenoValue_Dereference(&value);
			}
			break;

			// Functions
		case OP_LDFUNC:
			{
				AdrenoFunction *function;

				opSize += 4;

				value.Value.String = (AdrenoString *)ctx->LoadedScript->Strings.NodeHeap[opcode->Value.I4].Value.Value;
				value.Type = AT_STRING;

				function = AdrenoVM_GetFunction(vm, ctx, &value);
				if (!function)
				{
					vm->State = ST_END;
					break;
				}

				value.GCFlags = GC_NONE;
				value.ReferenceCounter = 1;
				value.Type = AT_FUNCTION;
				value.Value.Function = function;

				if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
				{
					vm->State = ST_END;
					break;
				}
			}
			break;

		case OP_CALL:
			{
				AdrenoReturnInfo *ri = (AdrenoReturnInfo *)AdrenoAlloc(sizeof(AdrenoReturnInfo));
				AdrenoFunction *function;

				if (!AdrenoStack_Pop(&ctx->Stack, &value))
				{
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);

				if (!rvalue)
				{
					vm->State = ST_END;
					break;
				}

				function = AdrenoVM_GetFunction(vm, ctx, rvalue);
				if (!function)
				{
					vm->State = ST_END;
					break;
				}
				
				ri->Script = ctx->LoadedScript;
				ri->Function = ctx->CurrentFunction;
				ri->Locals = ctx->Locals;
				ri->InstructionPointer = ctx->InstructionPointer;
				
				value.Type = AT_RETURNINFO;
				value.GCFlags = GC_COLLECT;
				value.ReferenceCounter = 1;
				value.Value.ReturnInfo = ri;

				if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
				{
					vm->State = ST_END;
					break;
				}

				AdrenoContext_AttachScript(ctx, function->Owner);
				AdrenoContext_SetFunction(ctx, function);

				opSize = 0;
				ctx->InstructionPointer = 0;
			}
			break;
		case OP_ENTER:
			{
				if (ctx->CurrentFunction->LocalsCount)
				{
					if (!AdrenoStack_Take(&ctx->Stack, &ctx->Locals, ctx->CurrentFunction->LocalsCount, ADRENOSTACK_CAN_EXPAND))
					{
						vm->State = ST_END;
						break;
					}
				}
				else
				{
					ctx->Locals = NULL;
				}
			}
			break;
		case OP_RET:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value))
				{
					vm->State = ST_END;
					break;
				}

				for (i = 0; i < ctx->CurrentFunction->LocalsCount; i++)
				{
					if (!AdrenoStack_Pop(&ctx->Stack, &value2))
					{
						vm->State = ST_END;
						break;
					}

					AdrenoValue_Dereference(&value2);
				}

				if (!AdrenoStack_Pop(&ctx->Stack, &value2) || value2.Type != AT_RETURNINFO)
				{
					vm->State = ST_END;
					break;
				}
				
				AdrenoContext_AttachScript(ctx, value2.Value.ReturnInfo->Script);
				AdrenoContext_SetFunction(ctx, value2.Value.ReturnInfo->Function);

				ctx->InstructionPointer = value2.Value.ReturnInfo->InstructionPointer + opSize;
				ctx->Locals = value2.Value.ReturnInfo->Locals;
				
				AdrenoValue_Dereference(&value2);
				opSize = 0;
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
	ctx->InstructionPointer = 0;
}

void AdrenoContext_Free(AdrenoContext *ctx)
{
	AdrenoStack_Clear(&ctx->Stack);
}
