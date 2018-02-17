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
        tapedeck.cc tapedeck.h
        breadboard.cc breadboard.h
        rom_images.cc rom_images.h
    )
    fips_dir(systems)
    fips_files(
        kc85.h kc85.cc
        kc85_exp.h kc85_exp.cc
        z1013.h z1013.cc
        z9001.h z9001.cc
        zx.h zx.cc
        cpc.h cpc.cc
        cpc_video.h cpc_video.cc
#        bbcmicro.h bbcmicro.cc
#        bbcmicro_video.h bbcmicro_video.cc
        atom.h atom.cc
    )
    fips_dir(roms)
    fips_generate(FROM rom_dumps.yml TYPE dump)
fips_end_module()
