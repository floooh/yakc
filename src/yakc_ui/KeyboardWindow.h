#pragma once
//------------------------------------------------------------------------------
/**
    @class KeyboardWindow
    @brief implements an onscreen keyboard
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class KeyboardWindow : public WindowBase {
    OryolClassDecl(KeyboardWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

    bool shift = false;
    bool caps_lock = false;
};

} // namespace YAKC
