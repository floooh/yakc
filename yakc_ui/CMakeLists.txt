fips_begin_module(yakc_ui)
    fips_vs_warning_level(3)
    fips_files(
        UI.cc UI.h
        HexInputWidget.h
        MemoryWindow.cc MemoryWindow.h
        WindowBase.cc WindowBase.h
        ImGuiMemoryEditor.h
        Disasm.cc Disasm.h
        DebugWindow.cc DebugWindow.h
        PIOWindow.cc PIOWindow.h
        ModuleWindow.cc ModuleWindow.h
        KeyboardWindow.cc KeyboardWindow.h
        z80dasm.cc z80dasm.h
    )
    fips_deps(yakc_core)
fips_end_module()
