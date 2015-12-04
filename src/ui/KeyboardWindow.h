#pragma once
//------------------------------------------------------------------------------
/**
    @class KeyboardWindow
    @brief implements an onscreen keyboard
*/
#include "ui/WindowBase.h"

class KeyboardWindow : public WindowBase {
    OryolClassDecl(KeyboardWindow);
public:
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

    bool shift = false;
    bool caps_lock = false;
};