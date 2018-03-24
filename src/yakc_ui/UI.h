#pragma once
//------------------------------------------------------------------------------
/**
    @class UI
    @brief imgui-based debugger UI
*/
#include "yakc_oryol/Audio.h"
#include "yakc_ui/WindowBase.h"
#include "yakc_oryol/FileLoader.h"
#include "Core/Time/TimePoint.h"
#include "Core/Containers/Array.h"
#include "IMUI/IMUI.h"

namespace YAKC {

class Audio;

class UI {
public:
    /// setup the UI
    void Setup(yakc& emu, Audio* audio);
    /// discard the UI
    void Discard();
    /// do one frame
    void OnFrame(yakc& emu);
    /// open a window
    void OpenWindow(yakc& emu, const Oryol::Ptr<WindowBase>& window);
    /// toggle the UI on/off
    void Toggle();
    /// toggle the onscreen keyboard on/off
    void ToggleKeyboard(yakc& emu);
    /// switch to dark UI theme
    void EnableDarkTheme();
    /// switch to light UI theme
    void EnableLightTheme();

    static ImVec4 DefaultTextColor;
    static ImVec4 EnabledColor;
    static ImVec4 DisabledColor;
    static ImVec4 OkColor;
    static ImVec4 WarnColor;
    static ImVec4 EnabledBreakpointColor;
    static ImVec4 DisabledBreakpointColor;
    static ImVec4 InvalidOpCodeColor;
    static ImU32 CanvasTextColor;
    static ImU32 CanvasLineColor;

    struct settings {
        bool crtEffect = false;
        bool colorTV = true;
        float crtWarp = 1.0f/64.0f;
    } Settings;

    Oryol::Duration EmulationTime;
    class FileLoader FileLoader;

    Oryol::TimePoint curTime;
    Oryol::Array<Oryol::Ptr<WindowBase>> windows;
    Oryol::Ptr<WindowBase> keyboardWindow;
    Oryol::Ptr<WindowBase> loadWindow;
    bool uiEnabled = false;
    Audio* audio;
};

} // namespace YAKC

