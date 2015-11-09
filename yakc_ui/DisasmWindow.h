#pragma once
//------------------------------------------------------------------------------
/**
    @class DisasmWindow
    @brief implement a simple disassembler ui
*/
#include "yakc_ui/WindowBase.h"
#include "Core/Containers/Array.h"

class DisasmWindow : public WindowBase {
    OryolClassDecl(DisasmWindow);
public:
    /// setup the window
    virtual void Setup(const yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

    /// draw window content, starting at given address
    void drawContent(const yakc::kc85& kc, yakc::uword start_addr, int num_lines);
    /// update the instruction history
    void updateHistory(const yakc::kc85& kc, yakc::uword addr);

    // a history of previously executed instructions
    static const int max_history_size = 8;
    Oryol::Array<yakc::uword> history;
};