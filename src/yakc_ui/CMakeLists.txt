fips_begin_module(yakc_ui)
    fips_vs_warning_level(3)
    fips_files(
        Util.h
        UI.cc UI.h
        HexInputWidget.h
        MemoryWindow.cc MemoryWindow.h
        MemoryMapWindow.cc MemoryMapWindow.h
        WindowBase.cc WindowBase.h
        ImGuiMemoryEditor.h
        DebugWindow.cc DebugWindow.h
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
    )
    fips_deps(IMUI yakc yakc_oryol soloud)
fips_end_module()
