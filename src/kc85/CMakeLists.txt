fips_begin_module(kc85)
    fips_vs_warning_level(3)
    fips_files(
        kc85.h kc85_video.h kc85_audio.h kc85_exp.h kc85_roms.h kc85_snapshot.h
        _dummy.cc
    )
    fips_deps(soloud)
fips_end_module()
