#pragma once
//------------------------------------------------------------------------------
/**
    @class PIOWindow
    @brief visualize the current PIO state
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class PIOWindow : public WindowBase {
    OryolClassDecl(PIOWindow);
public:
    /// constructor
    PIOWindow(const char* name, z80pio_t* pio);
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

    const char* Name = nullptr;
    z80pio_t* PIO = nullptr;
};

} // namespace YAKC
