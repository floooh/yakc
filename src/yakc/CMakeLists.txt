fips_begin_module(yakc)
    fips_vs_warning_level(3)
    fips_files(yakc.h yakc.cc)
    fips_dir(core)
    fips_files(
        core.h core.cc 
        memory.h memory.cc
        counter.h
        clock.h clock.cc
        sound.h sound.cc
        system_bus.h system_bus.cc
        filesystem.h filesystem.cc
    )
    fips_dir(chips)
    fips_files(
        z80.h z80.cc 
        z80int.h z80int.cc
        z80pio.h z80pio.cc
        z80ctc.h z80ctc.cc 
        cpudbg.h cpudbg.cc
        mos6502.h mos6502.cc
        i8255.h i8255.cc
        mc6845.h mc6845.cc
        mc6847.h mc6847.cc
        ay8910.h ay8910.cc
    )
    fips_generate(FROM z80_opcodes.py SOURCE z80_opcodes.cc)
    fips_dir(peripherals)
    fips_files(
        crt.h crt.cc
        beeper.h beeper.cc
        speaker.h speaker.cc
        tapedeck.h tapedeck.cc
    )
    fips_dir(systems)
    fips_files(
        rom_images.h rom_images.cc
        kc85.h kc85.cc 
        kc85_video.h kc85_video.cc
        kc85_audio.h kc85_audio.cc 
        kc85_exp.h kc85_exp.cc
        z1013.h z1013.cc
        z9001.h z9001.cc
        zx.h zx.cc
        cpc.h cpc.cc
        cpc_video.h cpc_video.cc
        bbcmicro.h bbcmicro.cc
        bbcmicro_video.h bbcmicro_video.cc
        atom.h atom.cc
        keybuffer.h keybuffer.cc
        breadboard.h
        snapshot.h snapshot.cc
    )
    fips_dir(roms)
    fips_generate(FROM rom_dumps.yml TYPE dump)
fips_end_module()
