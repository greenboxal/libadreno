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

#ifndef ADRENOCONFIG_H
#define ADRENOCONFIG_H

// Memory
#define USE_MALLOC 1
#define USE_MEMORY_MANAGER 1

#ifdef _DEBUG
#define MEMORYMANAGER_LOG 1
#endif

// Stack
#define ADRENOSTACK_DEFAULT_STACK 32
#define ADRENOSTACK_CAN_EXPAND 1
#define ADRENOSTACK_EXPANSION_FACTOR 5

// Memory Pool
#define ADRENOMP_USE_LINKED_LIST
#define ADRENOMP_PAGE_SIZE 4096

// This will force the use of libadreno memory manager for allocate memory for the pool
//#define ADRENOMP_FORCE_MEMMGR_ALLOC

// Bitarray
#define BITARRAY_USE_ANCHOR_OFFSET

#endif
