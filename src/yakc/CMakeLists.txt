fips_begin_app(yakc windowed)
    fips_files(
        Main.cc
        Draw.h Draw.cc
        Audio.h Audio.cc
        KC85Oryol.h
        kc85_audiosource.h kc85_audiosource.cc
    )
    oryol_shader(yakc_shaders.shd)
    if (YAKC_UI)
        fips_deps(ui IMUI)
    endif()
    fips_deps(Gfx HTTP Input Assets soloud roms)
fips_end_app()
