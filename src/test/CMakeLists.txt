if (FIPS_UNITTESTS)
fips_begin_unittest(yakc_test)
    fips_vs_warning_level(3)
    fips_files(
        filesystem_test.cc memory_test.cc daisychain_test.cc        
        mos6522_test.cc
        mos6502_test.cc
        z80_test.cc z80pio_test.cc
        zex_test.cc nestest_test.cc
    )
    fips_generate(FROM dump.yml TYPE dump)
    fips_generate(FROM nestest.log.txt TYPE nestestlog HEADER nestestlog.h)
    fips_deps(Core yakc)
fips_end_unittest()
endif()

