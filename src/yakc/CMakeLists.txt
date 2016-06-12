fips_begin_module(yakc)
    fips_vs_warning_level(3)
    fips_files(
        core.h core.cc memory.cc memory.h clock.h clock.cc
        z80.cc z80.h z80int.cc z80int.h z80pio.cc z80pio.h z80ctc.cc z80ctc.h z80dbg.cc z80dbg.h
        kc85.h kc85.cc kc85_video.h kc85_video.cc kc85_audio.h kc85_audio.cc
        kc85_exp.h kc85_exp.cc kc85_roms.h kc85_roms.cc
        kc85_snapshot.h kc85_snapshot.cc
        z1013.h z1013.cc
        z9001.h z9001.cc
        keybuffer.h keybuffer.cc
        breadboard.h
        yakc.h yakc.cc
    )
    fips_generate(FROM z80_opcodes.py SOURCE z80_opcodes.cc)
    fips_dir(roms)
    fips_generate(FROM roms.yml TYPE dump)
fips_end_module()
