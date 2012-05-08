#ifndef ADRENOEMIT_H
#define ADRENOEMIT_H

#include "AdrenoVM.h"

#ifdef __cplusplus
extern "C"
{
#endif

	extern unsigned int AdrenoEmit_AddString(AdrenoScript *script, wchar_t *string);
	extern AdrenoFunction *AdrenoEmit_CreateFunction(AdrenoScript *script, wchar_t *name);
	extern void AdrenoEmit_Expand(AdrenoFunction *function, int size);
	extern void AdrenoEmit_EmitOp(AdrenoFunction *function, AdrenoOpcodes op);
	extern void AdrenoEmit_EmitOp2_I1(AdrenoFunction *function, AdrenoOpcodes op, unsigned char val);
	extern void AdrenoEmit_EmitOp2_I2(AdrenoFunction *function, AdrenoOpcodes op, unsigned short val);
	extern void AdrenoEmit_EmitOp2_I4(AdrenoFunction *function, AdrenoOpcodes op, unsigned int val);
	extern void AdrenoEmit_EmitJump(AdrenoFunction *function, wchar_t *name);
	extern unsigned int AdrenoEmit_SetLabel(AdrenoFunction *fnc, wchar_t *name);
	extern int AdrenoEmit_ResolveJumps(AdrenoFunction *function);

#ifdef __cplusplus
}
#endif

#endif
