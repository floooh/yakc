#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::KC85IOWindow
    @brief KC85 specific IO port information
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class KC85IOWindow : public WindowBase {
    OryolClassDecl(KC85IOWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
