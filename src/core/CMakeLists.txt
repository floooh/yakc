fips_begin_module(core)
    fips_vs_warning_level(3)
    fips_files(
        memory.h clock.h common.h _dummy.cc
        z80int.h z80.h z80pio.h z80ctc.h z80dbg.h
        kc85.h kc85_video.h kc85_audio.h kc85_exp.h kc85_roms.h
        kc85_snapshot.h
    )
    if (NOT YAKC_Z80_DECODER)
        fips_files(z80_switch.h)
        # fips_generate(FROM opcodes.py HEADER opcodes.h)
        fips_generate(FROM z80_opcodes.py HEADER z80_opcodes.h)
    else()
        fips_files(z80_decoder.h)
    endif()
fips_end_module()
