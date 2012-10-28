set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/Debug/libadreno)

file (GLOB BASE_SRCS *.c)
file (GLOB AIL_SRCS ail/*.c)
file (GLOB CRYPT_SRCS crypt/*.c)
file (GLOB UTILS_SRCS utils/*.c)
file (GLOB VM_SRCS vm/*.c)

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
)

source_group (ail FILES ${AIL_SRCS})
source_group (crypt FILES ${CRYPT_SRCS})
source_group (utils FILES ${UTILS_SRCS})
source_group (vm FILES ${VM_SRCS})

source_group (include/ail FILES ${BASE_HDRS})
source_group (include/ail FILES ${AIL_HDRS})
source_group (include/ail FILES ${CRYPT_HDRS})
source_group (include/ail FILES ${UTILS_HDRS})
source_group (include/ail FILES ${VM_HDRS})
