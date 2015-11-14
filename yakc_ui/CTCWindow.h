#pragma once
//------------------------------------------------------------------------------
/**
    @class CTCWindow
    @brief visualize Z80 CTC state
*/
#include "yakc_ui/WindowBase.h"

class CTCWindow : public WindowBase {
    OryolClassDecl(CTCWindow);
public:
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;
};

