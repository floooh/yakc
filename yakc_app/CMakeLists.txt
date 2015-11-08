fips_begin_app(yakc_app windowed)
    fips_files(
        main.cc
        util.h
        draw.h draw.cc
    )
    oryol_shader(shaders.shd)
    if (YAKC_UI)
        fips_deps(yakc_ui)
    endif()
    fips_deps(Gfx Input IMUI yakc_core)
fips_end_app()
