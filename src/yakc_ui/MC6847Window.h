#pragma once
//------------------------------------------------------------------------------
/**
    @class MC6847Window
    @brief visualize MC6847 video display controller state
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class MC6847Window : public WindowBase {
    OryolClassDecl(MC6847Window);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
