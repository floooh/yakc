#pragma once
//------------------------------------------------------------------------------
/**
    @class LoadWindow
    @brief UI for loading KCC files from web server
*/
#include "yakc_ui/WindowBase.h"
#include "yakc_ui/FileLoader.h"
#include "Core/String/String.h"
#include "IO/Core/IOQueue.h"

class LoadWindow : public WindowBase {
    OryolClassDecl(LoadWindow);
public:
    /// set file loader
    void SetFileLoader(FileLoader* loader);
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

private:
    FileLoader* loader = nullptr;
};
