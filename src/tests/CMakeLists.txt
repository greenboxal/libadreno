set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/tests)

file (GLOB BASE_SRCS *.c *.cpp)

add_executable (tests ${BASE_SRCS})
target_link_libraries (tests adreno eaparser UnitTest++)
# cotire (tests)

source_group ("tests" FILES ${BASE_SRCS})
