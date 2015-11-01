#pragma once
//------------------------------------------------------------------------------
/**
    @class cpu_window
    @brief display and edit CPU state
*/
#include "ui_window.h"
#include "ui_hexinput.h"

class cpu_window : public window {
    OryolClassDecl(cpu_window);
public:
    /// constructor
    virtual void setup(const yakc::kc85& kc) override;
    /// draw method
    virtual bool draw(yakc::kc85& kc) override;

    hexinput reg_widget[yakc::z80::reg::num];
};