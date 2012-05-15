#include <adreno/vm/vm.h>

#include <memory.h>

void AdrenoVM_Initialize(AdrenoVM *vm)
{
	AdrenoHashtable_Initialize(&vm->GlobalFunctions, AdrenoHashtable_Hash_Fnv, AdrenoHashtable_Len_String);

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

void AdrenoVM_LoadValue(AdrenoValue **loadTo, AdrenoValue *tmp, AdrenoValue *value, int forceRef)
{
	if (value->Type == AT_REFERENCE || forceRef)
	{
		*loadTo = tmp;
		AdrenoValue_CreateReference(tmp, value);
	}
	else
	{
		*loadTo = value;
	}
}

void AdrenoVM_AddAPIFunction(AdrenoVM *vm, char *name, AdrenoAPIFunction function)
{
	AdrenoFunction *f = (AdrenoFunction *)AdrenoAlloc(sizeof(AdrenoFunction));

	f->Type = AF_API;
	f->APIFunction = function;
	f->GCFlags = GC_FREE;

	AdrenoHashtable_Set(&vm->GlobalFunctions, name, f);
}

void AdrenoVM_Run(AdrenoVM *vm, AdrenoContext *ctx)
{
	AdrenoValue value, value2, value3;
	AdrenoValue *rvalue, *rvalue2, *rvalue3, *rvalue4;
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
		case OP_POP_S:
			{
				i4 = opcode->Value.I4;
				opSize += 4;
				
				for (i = 0; i < i4; i++)
				{
					if (!AdrenoStack_Pop(&ctx->Stack, &value))
					{
						vm->Error = ERR_STACK_UNDERFLOW;
						vm->State = ST_END;
						break;
					}

					AdrenoValue_Dereference(&value);
				}
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

				AdrenoVM_LoadValue(&rvalue, &value, &ctx->Locals[index], prefix == P_REFERENCE);

				if (!AdrenoStack_Push(&ctx->Stack, rvalue, ADRENOSTACK_CAN_EXPAND))
				{
					vm->Error = ERR_STACK_OVERFLOW;
					vm->State = ST_END;
					break;
				}
			}
			break;

			// Arguments
		case OP_LDARG_0:
		case OP_LDARG_1:
		case OP_LDARG_2:
		case OP_LDARG_3:
		case OP_LDARG_S:
			{
				int index;

				if (op >= OP_LDARG_0 && op <= OP_LDARG_3)
				{
					index = op - OP_LDARG_0;
				}
				else
				{
					opSize += 4;
					index = opcode->Value.I4;
				}

				rvalue = AdrenoContext_GetArgument(ctx, index);

				if (!rvalue)
				{
					vm->Error = ERR_INVALID_ARGUMENT;
					vm->State = ST_END;
					break;
				}

				rvalue->ReferenceCounter++;
				if (!AdrenoStack_Push(&ctx->Stack, rvalue, ADRENOSTACK_CAN_EXPAND))
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
				AdrenoValue_LoadNull(&value);

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
				
				AdrenoValue_LoadInteger(&value, i4);

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

				rvalue = (AdrenoValue *)ctx->LoadedScript->Strings.NodeHeap[opcode->Value.I4].Value.Value;
				AdrenoValue_CreateReference(&value, rvalue);

				if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
				{
					vm->Error = ERR_STACK_OVERFLOW;
					vm->State = ST_END;
					break;
				}
			}
			break;

			// Arrays
		case OP_NEWARR:
			{
				AdrenoValue_LoadArray(&value);

				if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
				{
					vm->Error = ERR_STACK_OVERFLOW;
					vm->State = ST_END;
					break;
				}
			}
			break;
		case OP_STELEM:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value3) || !AdrenoStack_Pop(&ctx->Stack, &value2) || !AdrenoStack_Pop(&ctx->Stack, &value))
				{
					vm->Error = ERR_STACK_UNDERFLOW;
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);
				rvalue2 = AdrenoValue_GetValue(&value2);
				rvalue3 = AdrenoValue_GetValue(&value3);
				
				if (ADRENOVALUE_IS_NULL_VALUE_PTR(rvalue) || ADRENOVALUE_IS_NULL_VALUE_PTR(rvalue2) || rvalue3 == NULL)
				{
					vm->Error = ERR_NULL_REFERENCE;
					vm->State = ST_END;
					break;
				}

				if (rvalue->Type == AT_ARRAY)
				{				
					if (rvalue->Value.Array->Type == AT_NULL)
					{
						rvalue->Value.Array->Type = rvalue2->Type;
					}

					if (rvalue->Value.Array->Type != rvalue2->Type)
					{
						vm->Error = ERR_FOREING_CAST;
						vm->State = ST_END;
						break;
					}

					if (ADRENOVALUE_IS_REF_TYPE(rvalue3))
					{
						rvalue4 = (AdrenoValue *)AdrenoAlloc(sizeof(AdrenoValue));
						AdrenoValue_CreateReference(rvalue4, rvalue3);
					}
					else
					{
						rvalue4 = (AdrenoValue *)AdrenoAlloc(sizeof(AdrenoValue));
						*rvalue4 = *rvalue3;
						rvalue4->GCFlags = (AdrenoGCFlags)(GC_FREE | GC_COLLECT);
					}

					AdrenoHashtable_Set(&rvalue->Value.Array->Array, rvalue2, rvalue4);

					AdrenoValue_Dereference(&value);
					AdrenoValue_Dereference(&value2);
					AdrenoValue_Dereference(&value3);
				}
				else
				{
					vm->Error = ERR_INVALID_OPERAND;
					vm->State = ST_END;
					break;
				}
			}
			break;
		case OP_LDELEM:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value2) || !AdrenoStack_Pop(&ctx->Stack, &value))
				{
					vm->Error = ERR_STACK_UNDERFLOW;
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);
				rvalue2 = AdrenoValue_GetValue(&value2);
				
				if (ADRENOVALUE_IS_NULL_VALUE_PTR(rvalue) || ADRENOVALUE_IS_NULL_VALUE_PTR(rvalue2))
				{
					vm->Error = ERR_NULL_REFERENCE;
					vm->State = ST_END;
					break;
				}

				if (rvalue->Type == AT_ARRAY)
				{
					if (rvalue->Value.Array->Type == AT_NULL)
					{
						rvalue->Value.Array->Type = rvalue2->Type;
					}

					if (rvalue->Value.Array->Type != rvalue2->Type)
					{
						vm->Error = ERR_FOREING_CAST;
						vm->State = ST_END;
						break;
					}

					if (!AdrenoHashtable_Get(&rvalue->Value.Array->Array, rvalue2, (void **)&rvalue3))
					{
						value3.Type = AT_NULL;
					}
					else
					{
						value3 = *rvalue3;
						rvalue3->GCFlags = (AdrenoGCFlags)(rvalue3->GCFlags & ~GC_COLLECT);
					}

					if (!AdrenoStack_Push(&ctx->Stack, &value3, ADRENOSTACK_CAN_EXPAND))
					{
						vm->Error = ERR_STACK_OVERFLOW;
						vm->State = ST_END;
						break;
					}

					AdrenoValue_Dereference(&value);
					AdrenoValue_Dereference(&value2);
				}
				else
				{
					vm->Error = ERR_INVALID_OPERAND;
					vm->State = ST_END;
					break;
				}
			}
			break;
		case OP_LDLEN:
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

				value.Type = AT_INTEGER;
				value.GCFlags = GC_NONE;
				value.ReferenceCounter = 1;

				if (rvalue->Type == AT_ARRAY)
				{
					value.Value.I4 = rvalue->Value.Array->Array.NodeCount;
				}
				else if (rvalue->Type == AT_STRING)
				{
					value.Value.I4 = rvalue->Value.String->Size;
				}
				else
				{
					vm->Error = ERR_INVALID_OPERAND;
					vm->State = ST_END;
					break;
				}
			}
			break;
		case OP_RMELEM:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value3) || !AdrenoStack_Pop(&ctx->Stack, &value2) || !AdrenoStack_Pop(&ctx->Stack, &value))
				{
					vm->Error = ERR_STACK_UNDERFLOW;
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);
				rvalue2 = AdrenoValue_GetValue(&value2);
				rvalue3 = AdrenoValue_GetValue(&value3);
				
				if (ADRENOVALUE_IS_NULL_VALUE_PTR(rvalue) || ADRENOVALUE_IS_NULL_VALUE_PTR(rvalue2) || rvalue3 == NULL)
				{
					vm->Error = ERR_NULL_REFERENCE;
					vm->State = ST_END;
					break;
				}

				if (rvalue->Type == AT_ARRAY)
				{
					if (rvalue->Value.Array->Type == AT_NULL)
					{
						rvalue->Value.Array->Type = rvalue2->Type;
					}

					if (rvalue->Value.Array->Type != rvalue2->Type)
					{
						vm->Error = ERR_FOREING_CAST;
						vm->State = ST_END;
						break;
					}

					AdrenoHashtable_Remove(&rvalue->Value.Array->Array, rvalue2);

					AdrenoValue_Dereference(&value);
					AdrenoValue_Dereference(&value2);
				}
				else
				{
					vm->Error = ERR_INVALID_OPERAND;
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
						int newSize =  rvalue->Value.String->Size + rvalue2->Value.String->Size;
						char *newString = (char *)AdrenoAlloc(newSize + 1);
						
						memcpy(&newString[0], rvalue->Value.String->Value, rvalue->Value.String->Size);
						memcpy(&newString[rvalue->Value.String->Size], rvalue2->Value.String->Value, rvalue2->Value.String->Size);
						newString[newSize] = 0;

						AdrenoValue_LoadString(&value3, newString, newSize, 2);
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
					ADRENOVM_MATHOP_DUAL_CHECKTYPE(AT_STRING)
					{
						value3.Type = AT_INTEGER;
						
						if (rvalue->Value.String->Size != rvalue2->Value.String->Size)
							value3.Value.I4 = 0;
						else
							value3.Value.I4 = memcmp(rvalue->Value.String->Value, rvalue2->Value.String->Value, rvalue->Value.String->Size) == 0;
					}
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
					ADRENOVM_MATHOP_DUAL_CHECKTYPE(AT_STRING)
					{
						value3.Type = AT_INTEGER;
						
						if (rvalue->Value.String->Size != rvalue2->Value.String->Size)
							value3.Value.I4 = 0;
						else
							value3.Value.I4 = memcmp(rvalue->Value.String->Value, rvalue2->Value.String->Value, rvalue->Value.String->Size) != 0;
					}
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
					vm->Error = ERR_STACK_UNDERFLOW;
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);

				if (!rvalue)
				{
					vm->Error = ERR_NULL_REFERENCE;
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

				rvalue = (AdrenoValue *)ctx->LoadedScript->Strings.NodeHeap[opcode->Value.I4].Value.Value;

				function = AdrenoVM_GetFunction(vm, ctx, rvalue);
				if (!function)
				{
					vm->Error = ERR_UNKNOWN_FUNCTION;
					vm->State = ST_END;
					break;
				}

				value.GCFlags = GC_NONE;
				value.ReferenceCounter = 1;
				value.Type = AT_FUNCTION;
				value.Value.Function = function;

				if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
				{
					vm->Error = ERR_STACK_OVERFLOW;
					vm->State = ST_END;
					break;
				}
			}
			break;

		case OP_CALL:
			{
				AdrenoFunction *function;

				if (!AdrenoStack_Pop(&ctx->Stack, &value))
				{
					vm->Error = ERR_STACK_UNDERFLOW;
					vm->State = ST_END;
					break;
				}

				rvalue = AdrenoValue_GetValue(&value);

				if (!rvalue)
				{
					vm->Error = ERR_NULL_REFERENCE;
					vm->State = ST_END;
					break;
				}

				function = AdrenoVM_GetFunction(vm, ctx, rvalue);
				if (!function)
				{
					vm->State = ST_END;
					break;
				}

				if (function->Type == AF_SCRIPT)
				{
					AdrenoReturnInfo *ri = (AdrenoReturnInfo *)AdrenoAlloc(sizeof(AdrenoReturnInfo));

					ri->Script = ctx->LoadedScript;
					ri->Function = ctx->CurrentFunction;
					ri->Locals = ctx->Locals;
					ri->InstructionPointer = ctx->InstructionPointer;
					ri->ArgumentsPointer = ctx->ArgumentsPointer;
				
					value.Type = AT_RETURNINFO;
					value.GCFlags = GC_COLLECT;
					value.ReferenceCounter = 1;
					value.Value.ReturnInfo = ri;

					if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
					{
						vm->Error = ERR_STACK_OVERFLOW;
						vm->State = ST_END;
						break;
					}

					AdrenoContext_AttachScript(ctx, function->Owner);
					AdrenoContext_SetFunction(ctx, function);

					opSize = 0;
					ctx->InstructionPointer = 0;
				}
				else if (function->Type == AF_API)
				{
					function->APIFunction(vm, ctx);
				}
			}
			break;
		case OP_ENTER:
			{
				if (ctx->CurrentFunction->LocalsCount)
				{
					if (!AdrenoStack_Take(&ctx->Stack, &ctx->Locals, ctx->CurrentFunction->LocalsCount, ADRENOSTACK_CAN_EXPAND))
					{
						vm->Error = ERR_STACK_OVERFLOW;
						vm->State = ST_END;
						break;
					}
				}
				else
				{
					ctx->Locals = NULL;
				}

				ctx->ArgumentsPointer = ctx->Stack.StackPointer + 2;
			}
			break;
		case OP_RET:
			{
				if (!AdrenoStack_Pop(&ctx->Stack, &value))
				{
					vm->Error = ERR_STACK_UNDERFLOW;
					vm->State = ST_END;
					break;
				}

				for (i = 0; i < ctx->CurrentFunction->LocalsCount; i++)
				{
					if (!AdrenoStack_Pop(&ctx->Stack, &value2))
					{
						vm->Error = ERR_STACK_UNDERFLOW;
						vm->State = ST_END;
						break;
					}

					AdrenoValue_Dereference(&value2);
				}

				if (!AdrenoStack_Pop(&ctx->Stack, &value2))
				{
					vm->Error = ERR_SCRIPT_END;
					vm->State = ST_END;
					break;
				}

				if (value2.Type != AT_RETURNINFO)
				{
					vm->Error = ERR_STACK_CORRUPTION;
					vm->State = ST_END;
					break;
				}
				
				AdrenoContext_AttachScript(ctx, value2.Value.ReturnInfo->Script);
				AdrenoContext_SetFunction(ctx, value2.Value.ReturnInfo->Function);

				ctx->InstructionPointer = value2.Value.ReturnInfo->InstructionPointer + opSize;
				ctx->ArgumentsPointer = value2.Value.ReturnInfo->ArgumentsPointer;
				ctx->Locals = value2.Value.ReturnInfo->Locals;

				if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
				{
					vm->Error = ERR_STACK_OVERFLOW;
					vm->State = ST_END;
					break;
				}
				
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

	if (vm->State == ST_END)
		AdrenoStack_Clear(&ctx->Stack);
}

void AdrenoVM_Free(AdrenoVM *vm)
{
	unsigned int i;

	for (i = 0; i < vm->GlobalFunctions.NodeCount; i++)
	{
		AdrenoFunction *fnc = (AdrenoFunction *)vm->GlobalFunctions.NodeHeap[i].Value.Value;
		
		if (fnc->Type == AF_SCRIPT && fnc->GCFlags & GC_COLLECT && fnc->Bytecode)
			AdrenoFree(fnc->Bytecode);

		if (fnc->GCFlags & GC_FREE)
			AdrenoFree(fnc);
	}

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

void AdrenoContext_SetFunctionByName(AdrenoContext *ctx, char *name)
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

AdrenoValue *AdrenoContext_GetArgument(AdrenoContext *ctx, int index)
{
	unsigned int ptr = ctx->ArgumentsPointer + index;

	if (ptr >= ctx->Stack.StackSize)
	{
		return NULL;
	}

	return &ctx->Stack.Stack[ptr];
}

void AdrenoContext_Free(AdrenoContext *ctx)
{
	AdrenoStack_Free(&ctx->Stack);
}
