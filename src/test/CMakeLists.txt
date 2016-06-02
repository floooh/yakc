if (FIPS_UNITTESTS)
fips_begin_unittest(yakc_test)
    fips_vs_warning_level(3)
    fips_files(memory_test.cc z80_test.cc daisychain_test.cc zex_test.cc)
    fips_generate(FROM zex.yml TYPE dump)
    fips_deps(Core yakc)
fips_end_unittest()
endif()

