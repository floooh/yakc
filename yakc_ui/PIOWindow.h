#pragma once
//------------------------------------------------------------------------------
/**
    @class PIOWindow
    @brief visualize the current PIO state
*/
#include "yakc_ui/HexInputWidget.h"
#include "yakc_ui/WindowBase.h"

class PIOWindow : public WindowBase {
    OryolClassDecl(PIOWindow);
public:
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

    HexInputWidget pioAData;
    HexInputWidget pioBData;
};
