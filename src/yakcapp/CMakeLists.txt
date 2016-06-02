fips_begin_app(yakcapp windowed)
    fips_files(
        Main.cc
    )
    if (YAKC_UI)
        fips_deps(yakc_ui)
    endif()
    fips_deps(yakc yakc_oryol)
fips_end_app()
