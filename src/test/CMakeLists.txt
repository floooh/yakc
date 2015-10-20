fips_begin_unittest(core)
    fips_vs_warning_level(3)
    fips_files(mem_test.cc cpu_test.cc)
    fips_deps(yakc_core)
fips_end_unittest()

