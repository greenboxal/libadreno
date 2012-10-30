set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/Debug/tests)

add_executable (tests tests.cpp)
target_link_libraries (tests adreno UnitTest++)
cotire (tests)
