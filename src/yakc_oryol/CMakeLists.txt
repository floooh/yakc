fips_begin_lib(yakc_oryol)
    fips_files(
        Draw.h Draw.cc
        Audio.h Audio.cc
        Disasm.h Disasm.cc
        FileLoader.h FileLoader.cc
        SnapshotStorage.h SnapshotStorage.cc
        z80dasm.h z80dasm.cc
        kc85_audiosource.h kc85_audiosource.cc

    )
    oryol_shader(yakc_shaders.shd)
    fips_deps(Gfx HTTP Input Assets soloud yakc)
fips_end_lib()
