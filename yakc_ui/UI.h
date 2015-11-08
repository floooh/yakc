#pragma once
//------------------------------------------------------------------------------
/**
    @class UI
    @brief imgui-based debugger UI
*/
#include "yakc_app/kc85_oryol.h"
#include "yakc_ui/WindowBase.h"
#include "Time/TimePoint.h"
#include "Core/Containers/Array.h"

class UI {
public:
    /// setup the UI
    void Setup(yakc::kc85& kc);
    /// discard the UI
    void Discard();
    /// do one frame
    void OnFrame(yakc::kc85& kc);
    /// open a window
    void OpenWindow(const yakc::kc85& kc, Oryol::Ptr<WindowBase> window);
private:
    Oryol::TimePoint curTime;
    Oryol::Array<Oryol::Ptr<WindowBase>> windows;
};
