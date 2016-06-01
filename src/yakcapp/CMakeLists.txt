fips_begin_app(yakcapp windowed)
    fips_files(
        Main.cc
        Draw.h Draw.cc
        Audio.h Audio.cc
        KC85Oryol.h
        kc85_audiosource.h kc85_audiosource.cc
    )
    oryol_shader(yakc_shaders.shd)
    if (YAKC_UI)
        fips_deps(yakc ui IMUI)
    endif()
    fips_deps(Gfx HTTP Input Assets soloud)
fips_end_app()
