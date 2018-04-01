#pragma once
//------------------------------------------------------------------------------
/**
    @class M6522Window
    @brief visualize 6522 VIA state
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class M6522Window : public WindowBase {
    OryolClassDecl(M6522Window);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
