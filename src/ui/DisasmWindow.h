#pragma once
//------------------------------------------------------------------------------
/**
    @class DisasmWindow
    @brief a disassembler window
*/
#include "ui/HexInputWidget.h"
#include "ui/WindowBase.h"

class DisasmWindow : public WindowBase {
    OryolClassDecl(DisasmWindow);
public:
    /// setup the window
    virtual void Setup(yakc::emu& emu) override;
    /// draw method
    virtual bool Draw(yakc::emu& emu) override;

    /// draw the main window content, starting at given address
    void drawMainContent(const yakc::emu& emu, yakc::uword start_addr, int num_lines);
    /// draw control buttons
    void drawControls();

    HexInputWidget startWidget;
    HexInputWidget lengthWidget;
};
