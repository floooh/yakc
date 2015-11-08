#pragma once
//------------------------------------------------------------------------------
/**
    @class CpuWindow
    @brief display and edit CPU state
*/
#include "yakc_ui/WindowBase.h"
#include "yakc_ui/HexInputWidget.h"

class CpuWindow : public WindowBase {
    OryolClassDecl(CpuWindow);
public:
    /// setup the window
    virtual void Setup(const yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

    HexInputWidget regWidget[yakc::z80::reg::num];
};