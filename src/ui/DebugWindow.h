#pragma once
//------------------------------------------------------------------------------
/**
    @class DebugWindow
    @brief implement the step-debugger window
*/
#include "ui/HexInputWidget.h"
#include "ui/WindowBase.h"
#include "Core/Containers/Array.h"

class DebugWindow : public WindowBase {
    OryolClassDecl(DebugWindow);
public:
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

    /// draw a 16-bit register widget
    void drawReg16(yakc::kc85& kc, yakc::z80dbg::reg r);
    /// draw an 8-bit register widget
    void drawReg8(yakc::kc85& kc, yakc::z80dbg::reg r);
    /// draw the register table
    void drawRegisterTable(yakc::kc85& kc);
    /// draw the main window content, starting at given address
    void drawMainContent(const yakc::kc85& kc, yakc::uword start_addr, int num_lines);
    /// draw control buttons
    void drawControls(yakc::kc85& kc);

    HexInputWidget regWidget[yakc::z80dbg::reg::num];
    HexInputWidget breakPointWidget;
};