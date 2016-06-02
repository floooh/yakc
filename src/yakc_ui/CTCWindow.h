#pragma once
//------------------------------------------------------------------------------
/**
    @class CTCWindow
    @brief visualize Z80 CTC state
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class CTCWindow : public WindowBase {
    OryolClassDecl(CTCWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
