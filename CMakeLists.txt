cmake_minimum_required (VERSION 2.6)
project (Adreno)

# Compile with maximum warnings
if (MSVC)
  if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string (REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif ()
elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -pedantic -g")
endif ()

include_directories (${PROJECT_SOURCE_DIR}/src)
add_subdirectory (src)

