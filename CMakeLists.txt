#
# project: yakc
#
cmake_minimum_required(VERSION 2.8)

# include the fips main cmake file
get_filename_component(FIPS_ROOT_DIR "../fips" ABSOLUTE)
include("${FIPS_ROOT_DIR}/cmake/fips.cmake")

option(YAKC_UI "Include the debugger UI" ON)
if (YAKC_UI)
    add_definitions("-DYAKC_UI=1")
endif()

fips_setup()
fips_project(yakc)
fips_include_directories(.)
fips_add_subdirectory(yakc_core)
fips_add_subdirectory(yakc_test)
if (YAKC_UI)
    fips_add_subdirectory(yakc_ui)
endif()
fips_add_subdirectory(yakc_app)
fips_finish()


