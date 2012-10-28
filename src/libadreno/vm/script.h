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

#ifndef ADRENOSCRIPT_H
#define ADRENOSCRIPT_H

#ifndef ADRENOVM_H
#error You should not include this file directly
#endif

typedef enum
{
	AF_SCRIPT,
	AF_API,
} AdrenoFunctionType;

struct adrenofunction
{
	AdrenoFunctionType Type;
	unsigned int Index;
	unsigned int NameIndex;
	unsigned int LocalsCount;

	AdrenoScript *Owner;

	AdrenoGCFlags GCFlags;
	unsigned char *Bytecode;
	unsigned int BytecodeSize;

	AdrenoAPIFunction APIFunction;
};

struct adrenoscript
{
	AdrenoGCFlags GCFlags;

	AdrenoHashtable Functions;
	AdrenoArray Strings;
};

#ifdef __cplusplus
extern "C"
{
#endif

	extern void AdrenoScript_Initialize(AdrenoScript *script);
	extern char *AdrenoScript_Save(AdrenoScript *script, unsigned int *size);
	extern AdrenoScript *AdrenoScript_Load(char *data);
	extern void AdrenoScript_Free(AdrenoScript *script);

#ifdef __cplusplus
}
#endif

#endif
