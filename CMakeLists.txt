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

# manual imports
fips_ide_group("Oryol")
fips_import_oryol_Core()
fips_import_oryol_Gfx()
fips_import_oryol_IO()
fips_import_oryol_HTTP()
fips_import_oryol_Messaging()
fips_import_oryol_Input()
fips_import_oryol_IMUI()
fips_import_oryol_Resource()
fips_import_oryol_Assets()
fips_import_oryol_Time()
fips_import_oryol_Synth()
fips_ide_group("Libs")
fips_import_fips_zlib_zlib()
fips_import_fips_glfw_glfw3()
fips_import_oryol_flextgl()
fips_import_oryol_ConvertUTF()
fips_import_fips_imgui_imgui()
fips_import_fips_unittestpp_unittestpp()
if (FIPS_PROFILING)
    fips_import_fips_remotery_Remotery()
endif()
fips_ide_group("")

fips_include_directories(src)

fips_add_subdirectory(src/roms)
fips_add_subdirectory(src/core)
fips_add_subdirectory(src/test)
if (YAKC_UI)
    fips_add_subdirectory(src/ui)
endif()
fips_add_subdirectory(src/yakc)
fips_finish()


