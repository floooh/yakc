#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::InfoWindow
    @brief show quick info about currently emulated system
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class InfoWindow : public WindowBase {
    OryolClassDecl(InfoWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
