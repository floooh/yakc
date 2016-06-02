#pragma once
//------------------------------------------------------------------------------
/**
    @class LoadWindow
    @brief UI for loading KCC files from web server
*/
#include "yakc_ui/WindowBase.h"
#include "yakc_ui/FileLoader.h"

namespace YAKC {

class LoadWindow : public WindowBase {
    OryolClassDecl(LoadWindow);
public:
    /// set file loader
    void SetFileLoader(FileLoader* loader);
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

private:
    FileLoader* loader = nullptr;
};

} // namespace YAKC
