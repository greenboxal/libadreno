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

    ${BASE_HDRS}
    ${AIL_HDRS}
    ${CRYPT_HDRS}
    ${UTILS_HDRS}
    ${VM_HDRS}

)

source_group ("Source Files\\ail" FILES ${AIL_SRCS})
source_group ("Source Files\\crypt" FILES ${CRYPT_SRCS})
source_group ("Source Files\\utils" FILES ${UTILS_SRCS})
source_group ("Source Files\\vm" FILES ${VM_SRCS})

source_group ("Header Files" FILES ${BASE_HDRS})
source_group ("Header Files\\ail" FILES ${AIL_HDRS})
source_group ("Header Files\\crypt" FILES ${CRYPT_HDRS})
source_group ("Header Files\\utils" FILES ${UTILS_HDRS})
source_group ("Header Files\\vm" FILES ${VM_HDRS})
