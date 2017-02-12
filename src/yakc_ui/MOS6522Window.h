#pragma once
//------------------------------------------------------------------------------
/**
    @class MOS6522Window
    @brief visualize 6522 VIA state
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class MOS6522Window : public WindowBase {
    OryolClassDecl(MOS6522Window);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
