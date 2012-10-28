cmake_minimum_required (VERSION 2.6)
project (Adreno)

if (MSVC)
	add_definitions (/D_SCL_SECURE_NO_WARNINGS)
	add_definitions (/D_CRT_SECURE_NO_WARNINGS)
	add_definitions (/D_CRT_SECURE_NO_DEPRECATE)
	add_definitions (/D_CRT_NONSTDC_NO_DEPRECATE)
endif ()

include_directories (${PROJECT_SOURCE_DIR}/src)
add_subdirectory (src)
