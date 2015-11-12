#pragma once
//------------------------------------------------------------------------------
/**
    @class UI
    @brief imgui-based debugger UI
*/
#include "yakc_app/KC85Oryol.h"
#include "yakc_ui/WindowBase.h"
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

    static const ImVec4 ColorText;
    static const ImVec4 ColorDetail;
    static const ImVec4 ColorDetailBright;
    static const ImVec4 ColorDetailDark;
    static const ImVec4 ColorBackground;
    static const ImVec4 ColorBackgroundLight;

private:
    Oryol::TimePoint curTime;
    Oryol::Array<Oryol::Ptr<WindowBase>> windows;
};
