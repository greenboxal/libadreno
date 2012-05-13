#ifndef ADRENOEMIT_H
#define ADRENOEMIT_H

#include <adreno/vm/vm.h>

#ifdef __cplusplus
extern "C"
{
#endif

	extern unsigned int AdrenoEmit_AddString(AdrenoScript *script, char *string, unsigned int len);
	extern AdrenoFunction *AdrenoEmit_CreateFunction(AdrenoScript *script, char *name);
	extern void AdrenoEmit_EmitOp(AdrenoFunction *function, unsigned char op);
	extern void AdrenoEmit_EmitOp2_I4(AdrenoFunction *function, unsigned char op, unsigned int val);
	extern void AdrenoEmit_EmitJump(AdrenoFunction *function, unsigned char op, char *name);
	extern unsigned int AdrenoEmit_SetLabel(AdrenoFunction *fnc, char *name);
	extern int AdrenoEmit_Finalize(AdrenoFunction *function);

#ifdef __cplusplus
}
#endif

#endif
