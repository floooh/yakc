fips_begin_app(yakc windowed)
    fips_files(
        Main.cc
        Draw.h Draw.cc
        Audio.h Audio.cc
        KC85Oryol.h
    )
    oryol_shader(shaders.shd)
    if (YAKC_UI)
        fips_deps(ui)
    endif()
    fips_deps(Gfx HTTP Input IMUI Assets soloud core roms )
fips_end_app()
