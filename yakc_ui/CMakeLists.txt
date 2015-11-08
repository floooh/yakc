fips_begin_module(yakc_ui)
    fips_vs_warning_level(3)
    fips_files(
        UI.cc UI.h
        CpuWindow.cc CpuWindow.h
        HexInputWidget.h
        MemoryWindow.cc MemoryWindow.h
        WindowBase.cc WindowBase.h
        ImGuiMemoryEditor.h
        Disasm.cc Disasm.h
        z80dasm.cc z80dasm.h
    )
fips_end_module()
