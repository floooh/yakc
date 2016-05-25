fips_begin_module(z80)
    fips_vs_warning_level(3)
    fips_files(
        z80int.h z80.h z80pio.h z80ctc.h z80dbg.h
        _dummy.cc
    )
    fips_generate(FROM z80_opcodes.py HEADER z80_opcodes.h)
fips_end_module()
