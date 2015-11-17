fips_begin_unittest(yakc_test)
    fips_vs_warning_level(3)
    fips_files(memory_test.cc z80_test.cc daisychain_test.cc)
    fips_deps(yakc_core)
fips_end_unittest()

