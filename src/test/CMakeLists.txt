if (FIPS_UNITTESTS)
fips_begin_unittest(yakc_test)
    fips_vs_warning_level(3)
    fips_files(
        memory_test.cc daisychain_test.cc        
        mos6502_test.cc
        z80_test.cc z80pio_test.cc
        zex_test.cc
    )
    fips_generate(FROM dump.yml TYPE dump)
    fips_deps(Core yakc)
fips_end_unittest()
endif()

