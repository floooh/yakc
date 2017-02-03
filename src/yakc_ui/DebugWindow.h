#pragma once
//------------------------------------------------------------------------------
/**
    @class DebugWindow
    @brief implement the step-debugger window
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class DebugWindow : public WindowBase {
    OryolClassDecl(DebugWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

    /// draw the register table (Z80)
    void drawZ80RegisterTable(yakc& emu);
    /// draw the register table (m6502)
    void draw6502RegisterTable(yakc& emu);
    
    /// draw the main window content, starting at given address
    void drawMainContent(yakc& emu, uword start_addr, int num_lines);
    /// draw control buttons
    void drawControls(yakc& emu);
};

} // namespace YAKC
