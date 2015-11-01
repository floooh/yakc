fips_begin_app(yakc_app windowed)
    fips_files(
        main.cc
        util.h
        ui.cc ui.h
        ui_hexinput.h
        ui_window.h ui_window.cc
        ui_cpu_window.h ui_cpu_window.cc
        ui_mem_window.h ui_mem_window.cc
        draw.h draw.cc
        imgui_memoryeditor.h
    )
    oryol_shader(shaders.shd)
    fips_deps(Gfx Input IMUI yakc_core)
fips_end_app()
