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

    /// draw a Z80 16-bit register widget
    void drawZ80Reg16(yakc& emu, cpudbg::z80reg r);
    /// draw an Z80 8-bit register widget
    void drawZ80Reg8(yakc& emu, cpudbg::z80reg r);
    /// draw the register table (Z80)
    void drawZ80RegisterTable(yakc& emu);

    /// draw a m6502 16-bit register widget
    void draw6502Reg16(yakc& emu, cpudbg::m6502reg r);
    /// draw an m6502 8-bit register widget
    void draw6502Reg8(yakc& emu, cpudbg::m6502reg r);
    /// draw the register table (m6502)
    void draw6502RegisterTable(yakc& emu);
    
    /// draw the main window content, starting at given address
    void drawMainContent(yakc& emu, uword start_addr, int num_lines);
    /// draw control buttons
    void drawControls(yakc& emu);

    HexInputWidget z80RegWidget[int(cpudbg::z80reg::num)];
    HexInputWidget m6502RegWidget[int(cpudbg::m6502reg::num)];
    HexInputWidget breakPointWidget;
};

} // namespace YAKC
