fips_begin_module(yakc_core)
    fips_vs_warning_level(3)
    fips_files(
        memory.h clock.h common.h _dummy.cc
        z80int.h z80.h z80pio.h z80ctc.h
        kc85.h kc85_video.h kc85_expansion.h kc85_module.h
    )
    fips_generate(FROM opcodes.py HEADER opcodes.h)
fips_end_module()
