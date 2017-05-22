#
# project: yakc
#
cmake_minimum_required(VERSION 2.8)
get_filename_component(FIPS_ROOT_DIR "../fips" ABSOLUTE)
include("${FIPS_ROOT_DIR}/cmake/fips.cmake")
fips_setup(PROJECT yakc)

option(YAKC_UI "Include the debugger UI" ON)
if (YAKC_UI)
    add_definitions("-DYAKC_UI=1")
endif()

if (FIPS_MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(SOLOUD_LIBMODPLUG OFF CACHE BOOL "Do not use libmodplug")
fips_ide_group("Oryol")
fips_import_oryol_Core()
fips_import_oryol_Gfx()
fips_import_oryol_IO()
fips_import_oryol_HttpFS()
fips_import_oryol_Input()
fips_import_oryol_imgui_IMUI()
fips_import_oryol_Resource()
fips_import_oryol_Assets()
fips_ide_group("ExtLibs")
fips_import_fips_zlib_zlib()
if (ORYOL_OPENGL AND NOT FIPS_RASPBERRYPI)
    fips_import_fips_glfw_glfw3()
endif()
fips_import_fips_imgui_imgui()
fips_import_fips_soloud_soloud()
fips_import_fips_unittestpp_unittestpp()
if (FIPS_PROFILING)
    fips_import_fips_remotery_Remotery()
endif()

fips_ide_group("YAKC")
fips_include_directories(src)
fips_add_subdirectory(src/yakc)
fips_add_subdirectory(src/test)
if (YAKC_UI)
    fips_add_subdirectory(src/yakc_ui)
endif()
fips_add_subdirectory(src/yakc_oryol)
fips_add_subdirectory(src/yakcapp)
fips_finish()



