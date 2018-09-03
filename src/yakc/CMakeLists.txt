fips_begin_module(yakc)
    fips_vs_warning_level(3)
    fips_files(yakc.h yakc.cc)
    fips_dir(util)
    fips_files(
        audiobuffer.cc audiobuffer.h
        core.h core.cc 
        debugger.cc debugger.h
        filesystem.h filesystem.cc
        audiobuffer.h audiobuffer.cc
        filetypes.h
        breadboard.cc breadboard.h
        rom_images.cc rom_images.h
    )
    fips_dir(systems)
    fips_files(
        systems.cc
        kc85.h kc85.cc
        z1013.h z1013.cc
        z9001.h z9001.cc
        zx.h zx.cc
        cpc.h cpc.cc
        atom.h atom.cc
        c64.h c64.cc
    )
    fips_dir(roms)
    fips_generate(FROM rom_dumps.yml TYPE dump)
fips_end_module()
