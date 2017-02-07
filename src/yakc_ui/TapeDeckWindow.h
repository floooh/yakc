#pragma once
//------------------------------------------------------------------------------
/**
    @class TapeDeckWindow
    @brief tape deck controls for system with real TAP loading
*/
#include "yakc_ui/WindowBase.h"
#include "yakc_oryol/FileLoader.h"

namespace YAKC {

class TapeDeckWindow : public WindowBase {
    OryolClassDecl(TapeDeckWindow);
public:
    /// constructor with pointer to FileLoader
    TapeDeckWindow(FileLoader* loader);
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

    FileLoader* fileLoader;
    float angle = 0.0f;
};

} // namespace YAKC
