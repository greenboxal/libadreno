set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/Debug/ailc)

add_executable (ailc ailc.c)
target_link_libraries (ailc adreno)
