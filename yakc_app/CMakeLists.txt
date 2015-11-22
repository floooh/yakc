if (FIPS_IOS OR FIPS_OSX)
    # IOS or OSX are picky about underscores in bundle app names
    set(appname "yakcapp")
else()
    set(appname "yakc_app")
endif()

fips_begin_app(${appname} windowed)
    fips_files(
        Main.cc
        Util.h
        Draw.h Draw.cc
        Audio.h Audio.cc
        KC85Oryol.h
    )
    oryol_shader(shaders.shd)
    if (YAKC_UI)
        fips_deps(yakc_ui)
    endif()
    fips_deps(Gfx HTTP Input IMUI Sound yakc_core)
fips_end_app()
