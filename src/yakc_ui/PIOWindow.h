#pragma once
//------------------------------------------------------------------------------
/**
    @class PIOWindow
    @brief visualize the current PIO state
*/
#include "yakc_ui/HexInputWidget.h"
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class PIOWindow : public WindowBase {
    OryolClassDecl(PIOWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

    HexInputWidget pioAData;
    HexInputWidget pioBData;
};

} // namespace YAKC
