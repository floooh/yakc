#pragma once
//------------------------------------------------------------------------------
/**
    @class LoadWindow
    @brief UI for loading KCC files from web server
*/
#include "yakc_ui/WindowBase.h"

class LoadWindow : public WindowBase {
    OryolClassDecl(LoadWindow);
public:
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;
};
