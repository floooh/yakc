#pragma once
//------------------------------------------------------------------------------
/**
    @class TapeDeckWindow
    @brief tape deck controls for system with real TAP loading
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class TapeDeckWindow : public WindowBase {
    OryolClassDecl(TapeDeckWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
