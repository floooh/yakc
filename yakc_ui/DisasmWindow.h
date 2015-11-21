#pragma once
//------------------------------------------------------------------------------
/**
    @class DisasmWindow
    @brief a disassembler window
*/
#include "yakc_ui/HexInputWidget.h"
#include "yakc_ui/WindowBase.h"

class DisasmWindow : public WindowBase {
    OryolClassDecl(DisasmWindow);
public:
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

    /// draw the main window content, starting at given address
    void drawMainContent(const yakc::kc85& kc, yakc::uword start_addr, int num_lines);
    /// draw control buttons
    void drawControls();

    HexInputWidget startWidget;
    HexInputWidget lengthWidget;
};
