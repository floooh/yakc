fips_begin_module(yakc_ui)
    fips_vs_warning_level(3)
    fips_files(
        Util.cc Util.h
        UI.cc UI.h
        MemoryWindow.cc MemoryWindow.h
        MemoryMapWindow.cc MemoryMapWindow.h
        WindowBase.cc WindowBase.h
        ImGuiMemoryEditor.h
        DebugWindow.cc DebugWindow.h
        z80dasm.cc z80dasm.h
        mos6502dasm.cc mos6502dasm.h
        Disasm.cc Disasm.h
        DisasmWindow.cc DisasmWindow.h
        PIOWindow.cc PIOWindow.h
        CTCWindow.cc CTCWindow.h
        ModuleWindow.cc ModuleWindow.h
        KeyboardWindow.cc KeyboardWindow.h
        LoadWindow.cc LoadWindow.h
        CommandWindow.cc CommandWindow.h
        AudioWindow.cc AudioWindow.h
        KC85IOWindow.cc KC85IOWindow.h
        InfoWindow.cc InfoWindow.h
        AY8910Window.cc AY8910Window.h
        MC6845Window.cc MC6845Window.h
        MC6847Window.cc MC6847Window.h
        I8255Window.cc I8255Window.h
    )
    fips_deps(IMUI yakc yakc_oryol soloud)
fips_end_module()
