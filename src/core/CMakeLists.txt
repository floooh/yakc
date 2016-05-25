fips_begin_module(core)
    fips_vs_warning_level(3)
    fips_files(
        memory.h clock.h common.h 
        _dummy.cc
    )
fips_end_module()
