#pragma once
//------------------------------------------------------------------------------
/**
    @class LoadWindow
    @brief UI for loading KCC files from web server
*/
#include "yakc_ui/WindowBase.h"
#include "yakc_ui/HexInputWidget.h"
#include "yakc_oryol/FileLoader.h"

namespace YAKC {

class LoadWindow : public WindowBase {
    OryolClassDecl(LoadWindow);
public:
    /// create with pointer to FileLoader
    LoadWindow(FileLoader* loader);
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

private:
    FileLoader* loader = nullptr;
    HexInputWidget startAddr;
    HexInputWidget endAddr;
    HexInputWidget execAddr;
};

} // namespace YAKC
