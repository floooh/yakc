#pragma once
//------------------------------------------------------------------------------
/**
    @class CTCWindow
    @brief visualize Z80 CTC state
*/
#include "ui/WindowBase.h"

class CTCWindow : public WindowBase {
    OryolClassDecl(CTCWindow);
public:
    /// setup the window
    virtual void Setup(yakc::emu& emu) override;
    /// draw method
    virtual bool Draw(yakc::emu& emu) override;
};

