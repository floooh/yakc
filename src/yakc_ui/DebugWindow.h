#pragma once
//------------------------------------------------------------------------------
/**
    @class DebugWindow
    @brief implement the step-debugger window
*/
#include "yakc_ui/HexInputWidget.h"
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class DebugWindow : public WindowBase {
    OryolClassDecl(DebugWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

    /// draw a 16-bit register widget
    void drawReg16(yakc& emu, z80dbg::reg r);
    /// draw an 8-bit register widget
    void drawReg8(yakc& emu, z80dbg::reg r);
    /// draw the register table
    void drawRegisterTable(yakc& emu);
    /// draw the main window content, starting at given address
    void drawMainContent(yakc& emu, uword start_addr, int num_lines);
    /// draw control buttons
    void drawControls(yakc& emu);

    HexInputWidget regWidget[z80dbg::reg::num];
    HexInputWidget breakPointWidget;
};

} // namespace YAKC
