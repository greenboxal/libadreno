set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/eacompiler)
set (EACOMPILER_SRC ${PROJECT_SOURCE_DIR}/src/eacompiler)

file (GLOB EA_SRCS *.c *.cpp)
file (GLOB EA_HDRS *.h)

set (EA_SRCS ${EA_SRCS} ${EACOMPILER_SRC}/compiler-parser.tab.cpp ${EACOMPILER_SRC}/compiler-tokens.cpp)
set (EA_HDRS ${EA_HDRS} ${EACOMPILER_SRC}/compiler-parser.tab.hpp)

add_custom_command(
	OUTPUT ${EACOMPILER_SRC}/compiler-tokens.cpp
	COMMAND ${FLEX_EXECUTABLE} 
	ARGS -o ${EACOMPILER_SRC}/compiler-tokens.cpp ${EACOMPILER_SRC}/compiler-tokens.l
	MAIN_DEPENDENCY compiler-tokens.l
)

add_custom_command(
	OUTPUT ${EACOMPILER_SRC}/compiler-parser.tab.cpp
	COMMAND ${BISON_EXECUTABLE}
	ARGS -v -d -o ${EACOMPILER_SRC}/compiler-parser.tab.cpp ${EACOMPILER_SRC}/compiler-parser.y
	MAIN_DEPENDENCY compiler-parser.y
)

set_source_files_properties (${EACOMPILER_SRC}/compiler-tokens.cpp GENERATED)
set_source_files_properties (${EACOMPILER_SRC}/compiler-parser.tab.cpp GENERATED)
set_source_files_properties (${EACOMPILER_SRC}/compiler-parser.tab.hpp GENERATED)

set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "compiler-parser.output")

if (MSVC)
	add_definitions(/DYY_NO_UNISTD_H)
endif ()

add_library (eacompiler ${EA_SRCS} ${EA_HDRS})
target_link_libraries (eacompiler adreno)
# cotire (eacompiler)

source_group ("eacompiler" FILES ${EA_SRCS} ${EA_HDRS})
