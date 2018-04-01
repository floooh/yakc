#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::M6581Window
    @brief show debug info about the M6581 SID chip
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class M6581Window : public WindowBase {
    OryolClassDecl(M6581Window);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
