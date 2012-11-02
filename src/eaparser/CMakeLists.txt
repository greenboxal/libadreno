set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/eacompiler)

file (GLOB EA_SRCS *.c *.cpp)
file (GLOB EA_HDRS *.h)

set (EA_SRCS ${EA_SRCS} compiler-parser.tab.cpp compiler-tokens.cpp)
set (EA_HDRS ${EA_HDRS} compiler-parser.tab.hpp)

add_custom_command(
	OUTPUT compiler-parser.tab.cpp
	COMMAND ${BISON_EXECUTABLE}
	ARGS -v -d -o ${PROJECT_SOURCE_DIR}/src/eaparser/compiler-parser.tab.cpp ${PROJECT_SOURCE_DIR}/src/eaparser/compiler-parser.y
	DEPENDS compiler-parser.y
)

add_custom_command(
	OUTPUT compiler-parser.tab.hpp
	COMMAND ${CMAKE_COMMAND} -E touch ${PROJECT_SOURCE_DIR}/src/eaparser/compiler-parser.tab.hpp
	DEPENDS compiler-parser.y
)

add_custom_command(
	OUTPUT compiler-tokens.cpp
	COMMAND ${FLEX_EXECUTABLE} 
	ARGS -o ${PROJECT_SOURCE_DIR}/src/eaparser/compiler-tokens.cpp ${PROJECT_SOURCE_DIR}/src/eaparser/compiler-tokens.l
	DEPENDS compiler-tokens.l
)

set_directory_properties (PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "compiler-parser.output")

add_library (eacompiler ${EA_SRCS} ${EA_HDRS})
target_link_libraries (eacompiler)
# cotire (eacompiler)

source_group ("eaparser" FILES ${EA_SRCS} ${EA_HDRS})
