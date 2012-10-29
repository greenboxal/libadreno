set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/Debug/tests)

file (GLOB TEST_SRCS *.c)

add_executable (tests ${TEST_SRCS})
target_link_libraries (tests adreno)
