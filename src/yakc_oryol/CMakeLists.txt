fips_begin_lib(yakc_oryol)
    fips_files(
        Draw.h Draw.cc
        Audio.h Audio.cc
        AudioSource.h AudioSource.cc
        Keyboard.h Keyboard.cc
        FileLoader.h FileLoader.cc
    )
    oryol_shader(yakc_shaders.shd)
    fips_deps(Gfx HttpFS Input Assets soloud yakc)
fips_end_lib()
