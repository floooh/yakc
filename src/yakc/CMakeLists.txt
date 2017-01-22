fips_begin_module(yakc)
    fips_vs_warning_level(3)
    fips_files(yakc.h yakc.cc)
    fips_dir(core)
    fips_files(
        core.h core.cc 
        memory.h memory.cc
        counter.h
        clock.h clock.cc
        system_bus.h system_bus.cc
    )
    fips_dir(devices)
    fips_files(
        crt.h crt.cc
        z80.h z80.cc 
        z80int.h z80int.cc
        z80pio.h z80pio.cc
        z80ctc.h z80ctc.cc 
        z80dbg.h z80dbg.cc
        mos6502.h mos6502.cc
        i8255.h i8255.cc
        mc6845.h mc6845.cc
        sound.h sound.cc
        speaker.h speaker.cc
        beeper.h beeper.cc
        ay8910.h ay8910.cc
    )
    fips_generate(FROM z80_opcodes.py SOURCE z80_opcodes.cc)
    # fips_generate(FROM mos6502_opcodes.py SOURCE mos6502_opcodes.cc)
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
        keybuffer.h keybuffer.cc
        breadboard.h
        snapshot.h snapshot.cc
    )
    fips_dir(roms)
    fips_generate(FROM rom_dumps.yml TYPE dump)
fips_end_module()
