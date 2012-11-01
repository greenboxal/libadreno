set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/Debug/tests)

file (GLOB BASE_SRCS *.c *.cpp)

add_executable (tests ${BASE_SRCS})
target_link_libraries (tests adreno UnitTest++)
# cotire (tests)

source_group ("tests" FILES ${BASE_SRCS})
