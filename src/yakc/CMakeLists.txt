fips_begin_module(yakc)
    fips_vs_warning_level(3)
    fips_files(yakc.h yakc.cc)
    fips_dir(core)
    fips_files(
        core.h core.cc 
        filesystem.h filesystem.cc
        audiobuffer.h audiobuffer.cc
        filetypes.h
    )
    fips_dir(chips)
    fips_files(
        cpudbg.h cpudbg.cc
#        mos6502.h mos6502.cc
#        mos6522.h mos6522.cc
#        i8255.h i8255.cc
#        mc6845.h mc6845.cc
#        mc6847.h mc6847.cc
#        ay8910.h ay8910.cc
    )
    fips_dir(peripherals)
    fips_files(
        crt.h crt.cc
        tapedeck.h tapedeck.cc
    )
    fips_dir(systems)
    fips_files(
        breadboard.h breadboard.cc
        rom_images.h rom_images.cc
#        kc85.h kc85.cc
#        kc85_video.h kc85_video.cc
#        kc85_audio.h kc85_audio.cc
#        kc85_exp.h kc85_exp.cc
        z1013.h z1013.cc
        z9001.h z9001.cc
        zx.h zx.cc
#        cpc.h cpc.cc
#        cpc_video.h cpc_video.cc
#        bbcmicro.h bbcmicro.cc
#        bbcmicro_video.h bbcmicro_video.cc
#        atom.h atom.cc
    )
    fips_dir(roms)
    fips_generate(FROM rom_dumps.yml TYPE dump)
fips_end_module()
