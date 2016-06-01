#
# project: yakc
#
if (NOT FIPS_IMPORT)
cmake_minimum_required(VERSION 2.8)
get_filename_component(FIPS_ROOT_DIR "../fips" ABSOLUTE)
include("${FIPS_ROOT_DIR}/cmake/fips.cmake")
fips_setup()
fips_project(yakc)
endif()

option(YAKC_UI "Include the debugger UI" ON)
if (YAKC_UI)
    add_definitions("-DYAKC_UI=1")
endif()

if (FIPS_MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# manual imports
if (NOT FIPS_IMPORT)
    set(SOLOUD_LIBMODPLUG OFF CACHE BOOL "Do not use libmodplug")
    fips_ide_group("Oryol")
    fips_import_oryol_Core()
    fips_import_oryol_Gfx()
    fips_import_oryol_IO()
    fips_import_oryol_HTTP()
    fips_import_oryol_Input()
    fips_import_oryol_imgui_IMUI()
    fips_import_oryol_Resource()
    fips_import_oryol_Assets()
    fips_ide_group("Libs")
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
        fips_add_subdirectory(src/ui)
    endif()
    fips_add_subdirectory(src/yakcapp)

else()
    # if imported, just put everything into a single module
    fips_begin_module(yakc)
        fips_ide_group("YAKC")
        fips_src(src/yakc)
        fips_files_ex(src/ui FileLoader.*)
        fips_src(src/yakc EXCEPT Main.cc)
        oryol_shader(yakc_shaders.shd)
        fips_deps(Gfx HTTP Input Assets soloud)
    fips_end_module()
endif()

if (NOT FIPS_IMPORT)
fips_finish()
endif()



