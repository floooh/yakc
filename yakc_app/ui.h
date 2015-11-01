#pragma once
//------------------------------------------------------------------------------
/**
    @class ui
    @brief imgui-based debugger UI
*/
#include "kc85_oryol.h"
#include "ui_window.h"
#include "Time/TimePoint.h"
#include "Core/Containers/Array.h"

class ui {
public:
    /// constructor
    ui();
    /// setup the UI
    void setup(yakc::kc85& kc);
    /// discard the UI
    void discard();
    /// do one frame
    void onframe(yakc::kc85& kc);
    /// open a window
    void open(const yakc::kc85& kc, Oryol::Ptr<window> window);
private:
    Oryol::TimePoint curTime;
    Oryol::Array<Oryol::Ptr<window>> windows;
};
