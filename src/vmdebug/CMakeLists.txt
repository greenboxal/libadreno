set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/Debug/vmdebug)

add_executable (vmdebug vmdebug.c)
target_link_libraries (vmdebug adreno)
