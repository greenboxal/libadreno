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

// Thread Local Storage Compiler Support
#cmakedefine HAS_THREAD_LOCAL
#cmakedefine THREAD_LOCAL @THREAD_LOCAL@

// Memory
#cmakedefine USE_MALLOC
#cmakedefine USE_MEMORY_MANAGER

#ifdef _DEBUG
#define MEMORYMANAGER_LOG 1
#endif

// Stack
#define ADRENOSTACK_DEFAULT_STACK @ADRENOSTACK_DEFAULT_STACK@
#define ADRENOSTACK_CAN_EXPAND @ADRENOSTACK_CAN_EXPAND@
#define ADRENOSTACK_EXPANSION_FACTOR @ADRENOSTACK_EXPANSION_FACTOR@

// Memory Pool
#cmakedefine ADRENOMP_USE_LINKED_LIST
#cmakedefine ADRENOMP_PAGE_SIZE @ADRENOMP_PAGE_SIZE@

// This will force the use of libadreno memory manager for allocate memory for the pool
#cmakedefine ADRENOMP_FORCE_MEMMGR_ALLOC

// Bitarray
#cmakedefine BITARRAY_USE_ANCHOR_OFFSET

// Hashtable
#cmakedefine ADRENOHT_DEFAULT_EXPANSION_FACTOR @ADRENOHT_DEFAULT_EXPANSION_FACTOR@

#endif
