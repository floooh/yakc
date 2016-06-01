fips_begin_module(ui)
    fips_vs_warning_level(3)
    fips_files(
        Util.h
        UI.cc UI.h
        HexInputWidget.h
        MemoryWindow.cc MemoryWindow.h
        MemoryMapWindow.cc MemoryMapWindow.h
        WindowBase.cc WindowBase.h
        ImGuiMemoryEditor.h
        Disasm.cc Disasm.h
        DebugWindow.cc DebugWindow.h
        DisasmWindow.cc DisasmWindow.h
        PIOWindow.cc PIOWindow.h
        CTCWindow.cc CTCWindow.h
        ModuleWindow.cc ModuleWindow.h
        KeyboardWindow.cc KeyboardWindow.h
        LoadWindow.cc LoadWindow.h
        CommandWindow.cc CommandWindow.h
        AudioWindow.cc AudioWindow.h
        FileLoader.cc FileLoader.h
        SnapshotStorage.cc SnapshotStorage.h
        z80dasm.cc z80dasm.h
    )
    fips_deps(yakc soloud)
fips_end_module()
