#pragma once
//------------------------------------------------------------------------------
/**
    @class DisasmWindow
    @brief a disassembler window
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class DisasmWindow : public WindowBase {
    OryolClassDecl(DisasmWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

    /// draw the main window content, starting at given address
    void drawMainContent(const yakc& emu, uint16_t start_addr, int num_lines);
    /// draw control buttons
    void drawControls();

    uint16_t startAddr = 0;
    uint16_t numLines = 64;
};

} // namespace YAKC

