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
