#pragma once
//------------------------------------------------------------------------------
/**
    @class I8255Window
    @brief visualize the i8255 state
*/
#include "yakc_ui/HexInputWidget.h"
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class I8255Window : public WindowBase {
    OryolClassDecl(I8255Window);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
