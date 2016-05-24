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
    virtual void Setup(yakc::emu& emu) override;
    /// draw method
    virtual bool Draw(yakc::emu& emu) override;

    /// draw a 16-bit register widget
    void drawReg16(yakc::emu& emu, yakc::z80dbg::reg r);
    /// draw an 8-bit register widget
    void drawReg8(yakc::emu& emu, yakc::z80dbg::reg r);
    /// draw the register table
    void drawRegisterTable(yakc::emu& emu);
    /// draw the main window content, starting at given address
    void drawMainContent(yakc::emu& emu, yakc::uword start_addr, int num_lines);
    /// draw control buttons
    void drawControls(yakc::emu& emu);

    HexInputWidget regWidget[yakc::z80dbg::reg::num];
    HexInputWidget breakPointWidget;
};