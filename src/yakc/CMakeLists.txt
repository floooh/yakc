fips_begin_module(yakc)
    fips_vs_warning_level(3)
    fips_files(
        core.h core.cc memory.h clock.h
        z80.h z80int.h z80pio.h z80ctc.h z80dbg.h
        kc85.h kc85_video.h kc85_audio.h kc85_exp.h kc85_roms.h kc85_snapshot.h
        z1013.h z1013_video.h
        breadboard.h
        yakc.h
    )
    fips_generate(FROM roms.yml TYPE dump)
    fips_generate(FROM z80_opcodes.py HEADER z80_opcodes.h)
fips_end_module()
