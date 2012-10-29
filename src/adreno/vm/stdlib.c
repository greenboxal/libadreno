/*	This file is part of libadreno.

    libadreno is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libadreno is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libadreno.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <adreno/vm/vm.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define APIFUNC(name) { #name, stdlib_##name }
#define APIDEF(name) static void stdlib_##name(AdrenoVM *vm, AdrenoContext *ctx)

struct functionPair
{
	char *name;
	AdrenoAPIFunction function;
};

APIDEF(print)
{
	AdrenoValue value, *vvalue, *rvalue;

	vvalue = AdrenoContext_GetArgument(ctx, 0);
	rvalue = AdrenoValue_GetValue(vvalue);

	if (!rvalue)
	{
		vm->Error = ERR_NULL_REFERENCE;
		vm->State = ST_END;
		return;
	}

	if (rvalue->Type == AT_STRING)
	{
		printf("%s\n", rvalue->Value.String->Value);
	}
	else if (rvalue->Type == AT_INTEGER)
	{
		printf("%d\n", rvalue->Value.I4);
	}
	
	AdrenoValue_Dereference(vvalue);

	AdrenoValue_LoadInteger(&value, 0);
	if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
	{
		vm->Error = ERR_STACK_OVERFLOW;
		vm->State = ST_END;
		return;
	}
}

APIDEF(inttostr)
{
	char tmp[128];
	AdrenoValue value, *vvalue, *rvalue;

	vvalue = AdrenoContext_GetArgument(ctx, 0);
	rvalue = AdrenoValue_GetValue(vvalue);

	if (!rvalue)
	{
		vm->Error = ERR_NULL_REFERENCE;
		vm->State = ST_END;
		return;
	}

	if (rvalue->Type == AT_INTEGER)
	{
		sprintf(tmp, "%d", rvalue->Value.I4);
		AdrenoValue_LoadString(&value, tmp, strlen(tmp), 1);
	}
	else
	{
		AdrenoValue_LoadNull(&value);
	}
	
	AdrenoValue_Dereference(vvalue);

	if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
	{
		vm->Error = ERR_STACK_OVERFLOW;
		vm->State = ST_END;
		return;
	}
}

APIDEF(strtoint)
{
	AdrenoValue value, *vvalue, *rvalue;

	vvalue = AdrenoContext_GetArgument(ctx, 0);
	rvalue = AdrenoValue_GetValue(vvalue);

	if (!rvalue)
	{
		vm->Error = ERR_NULL_REFERENCE;
		vm->State = ST_END;
		return;
	}

	if (rvalue->Type == AT_STRING)
	{
		AdrenoValue_LoadInteger(&value, strtoul(rvalue->Value.String->Value, NULL, 0));
	}
	else
	{
		AdrenoValue_LoadNull(&value);
	}
	
	AdrenoValue_Dereference(vvalue);

	if (!AdrenoStack_Push(&ctx->Stack, &value, ADRENOSTACK_CAN_EXPAND))
	{
		vm->Error = ERR_STACK_OVERFLOW;
		vm->State = ST_END;
		return;
	}
}

static struct functionPair StdlibFunctions[] =
{
	APIFUNC(strtoint),
	APIFUNC(print),
	APIFUNC(inttostr),
	{ NULL, NULL }
};

void AdrenoVM_LoadStdlib(AdrenoVM *vm)
{
	struct functionPair *pair = &StdlibFunctions[0];

	while(pair->name)
	{
		AdrenoVM_AddAPIFunction(vm, pair->name, pair->function);

		pair++;
	}
}
