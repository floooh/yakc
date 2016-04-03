#pragma once
//------------------------------------------------------------------------------
/**
    @class UI
    @brief imgui-based debugger UI
*/
#include "yakc/KC85Oryol.h"
#include "ui/WindowBase.h"
#include "ui/FileLoader.h"
#include "ui/SnapshotStorage.h"
#include "Time/TimePoint.h"
#include "Core/Containers/Array.h"
#include "IMUI/IMUI.h"

class UI {
public:
    /// setup the UI
    void Setup(yakc::kc85& kc);
    /// discard the UI
    void Discard();
    /// do one frame
    void OnFrame(yakc::kc85& kc);
    /// open a window
    void OpenWindow(yakc::kc85& kc, const Oryol::Ptr<WindowBase>& window);
    /// toggle the UI on/off
    void Toggle();
    /// switch to dark UI theme
    void EnableDarkTheme();
    /// switch to light UI theme
    void EnableLightTheme();

    static ImVec4 DefaultTextColor;
    static ImVec4 EnabledColor;
    static ImVec4 DisabledColor;
    static ImVec4 EnabledBreakpointColor;
    static ImVec4 DisabledBreakpointColor;
    static ImVec4 InvalidOpCodeColor;
    static ImU32 CanvasTextColor;
    static ImU32 CanvasLineColor;

    struct settings {
        bool crtEffect = false;
        bool colorTV = true;
        float crtWarp = 1.0f/64.0f;
        int cpuSpeed = 1;
    } Settings;

private:
    FileLoader fileLoader;
    SnapshotStorage snapshotStorage;
    Oryol::TimePoint curTime;
    Oryol::Array<Oryol::Ptr<WindowBase>> windows;
    bool uiEnabled = false;
    bool helpOpen = true;
    bool darkThemeEnabled = true;
    bool lightThemeEnabled = false;
    bool imguiAntiAliasedLines = true;
    ImGuiStyle darkTheme;
    ImGuiStyle lightTheme;
};
