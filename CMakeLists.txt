cmake_minimum_required (VERSION 2.6)
project (Adreno)

# Set a default build type if none was specified
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message (STATUS "Setting build type to 'Debug' as none was specified.")
  set (CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  set_property (CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif ()

# Compile with maximum warnings
if (MSVC)
  if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string (REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif ()

  # Except M$ only deprecations and warnings
  add_definitions (/D_SCL_SECURE_NO_WARNINGS)
  add_definitions (/D_CRT_SECURE_NO_WARNINGS)
  add_definitions (/D_CRT_SECURE_NO_DEPRECATE)
  add_definitions (/D_CRT_NONSTDC_NO_DEPRECATE)
elseif (CMAKE_COMPILER_IS_GNUCC)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Wextra -Wno-unused-parameter -g")
endif ()

if (CMAKE_BUILD_TYPE EQUAL Debug)
  add_definitions (-D_DEBUG)
endif ()

include_directories (${PROJECT_SOURCE_DIR}/src)
add_subdirectory (src)

