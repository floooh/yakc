fips_begin_module(core)
    fips_vs_warning_level(3)
    fips_files(
        memory.h clock.h common.h 
        z80int.h z80.h z80pio.h z80ctc.h z80dbg.h
        _dummy.cc
    )
    if (NOT YAKC_Z80_DECODER)
        fips_generate(FROM z80_opcodes.py HEADER z80_opcodes.h)
    else()
        fips_files(z80_decoder.h)
    endif()
fips_end_module()
