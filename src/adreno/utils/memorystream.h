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

#ifndef ADRENOMS_H
#define ADRENOMS_H

#include <stddef.h>

typedef struct
{
	unsigned char *buffer;
	size_t bufferPosition, bufferSize, bufferMaxSize;
	size_t expansionFactor;
} AdrenoMS;

#ifdef __cplusplus
extern "C"
{
#endif

	extern void AdrenoMS_Open(AdrenoMS *ms);
	extern int AdrenoMS_Write(AdrenoMS *ms, unsigned char *buffer, size_t offset, size_t size);
	extern int AdrenoMS_Read(AdrenoMS *ms, unsigned char *buffer, size_t offset, size_t size);
	extern int AdrenoMS_Seek(AdrenoMS *ms, size_t origin, size_t offset);
	extern unsigned char *AdrenoMS_Clone(AdrenoMS *ms);
	extern void AdrenoMS_Close(AdrenoMS *ms);

#ifdef __cplusplus
}
#endif

#endif
