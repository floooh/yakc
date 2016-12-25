#pragma once
//------------------------------------------------------------------------------
/**
    @class AY8910Window
    @brief visualize AY-3-8910 sound chip state
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class AY8910Window : public WindowBase {
    OryolClassDecl(AY8910Window);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
