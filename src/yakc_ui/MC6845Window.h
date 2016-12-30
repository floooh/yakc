#pragma once
//------------------------------------------------------------------------------
/**
    @class MC6845Window
    @brief visualize MC6845 cathode-ray-tube-controller state
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class MC6845Window : public WindowBase {
    OryolClassDecl(MC6845Window);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
