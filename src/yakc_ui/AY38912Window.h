#pragma once
//------------------------------------------------------------------------------
/**
    @class AY8912Window
    @brief visualize AY-3-8912 sound chip state
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class AY38912Window : public WindowBase {
    OryolClassDecl(AY38912Window);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
