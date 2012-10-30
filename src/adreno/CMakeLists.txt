set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/Debug/libadreno)

# Config file
# Memory
option (USE_MALLOC "Use malloc" ON)
option (USE_MEMORY_MANAGER "Use builtin memory manager" ON)

# Stack
set (ADRENOSTACK_DEFAULT_STACK 32 CACHE STRING "Initial AdrenoVM stack size")
set (ADRENOSTACK_CAN_EXPAND 1 CACHE STRING "AdrenoVM stack is expandable")
set (ADRENOSTACK_EXPANSION_FACTOR 5 CACHE STRING "AdrenoVM stack expansion factor")

# Memory Pool
option (ADRENOMP_USE_LINKED_LIST "Use linked-list for memory pool(faster)" ON)

# This was suposed to be detected by the system
set (ADRENOMP_PAGE_SIZE 4096 CACHE STRING "Adreno page size")

option (ADRENOMP_FORCE_MEMMGR_ALLOC "Force the use of libadreno memory manager for allocate memory for the pool" ON)

# Bitarray
option (BITARRAY_USE_ANCHOR_OFFSET "Cache last used bit position(faster)" ON)

# Hashtable
set (ADRENOHT_DEFAULT_EXPANSION_FACTOR 10 CACHE STRING "Hashtable expansion factor")

configure_file (${CMAKE_SOURCE_DIR}/src/adreno/config.h.cmake ${CMAKE_SOURCE_DIR}/src/adreno/config.h)

file (GLOB BASE_SRCS *.c *.cpp)
file (GLOB AIL_SRCS ail/*.c ail/*.cpp)
file (GLOB CRYPT_SRCS crypt/*.c crypt/*.cpp)
file (GLOB UTILS_SRCS utils/*.c utils/*.cpp)
file (GLOB VM_SRCS vm/*.c vm/*.cpp)

file (GLOB BASE_HDRS *.h)
file (GLOB AIL_HDRS ail/*.h)
file (GLOB CRYPT_HDRS crypt/*.h)
file (GLOB UTILS_HDRS utils/*.h)
file (GLOB VM_HDRS vm/*.h)

add_library (adreno
    ${BASE_SRCS}
    ${AIL_SRCS}
    ${CRYPT_SRCS}
    ${UTILS_SRCS}
    ${VM_SRCS}

    ${BASE_HDRS}
    ${AIL_HDRS}
    ${CRYPT_HDRS}
    ${UTILS_HDRS}
    ${VM_HDRS}

)
cotire (adreno)

source_group ("adreno" FILES ${BASE_SRCS} ${BASE_HDRS})
source_group ("adreno\\ail" FILES ${AIL_SRCS} ${AIL_HDRS})
source_group ("adreno\\crypt" FILES ${CRYPT_SRCS} ${CRYPT_HDRS})
source_group ("adreno\\utils" FILES ${UTILS_SRCS} ${UTILS_HDRS})
source_group ("adreno\\vm" FILES ${VM_SRCS} ${VM_HDRS})
