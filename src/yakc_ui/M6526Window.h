#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::M6526Window
    @brief show debug info about M6526 CIA
*/
#include "yakc_ui/WindowBase.h"
#include "yakc/util/breadboard.h"

namespace YAKC {

class M6526Window : public WindowBase {
    OryolClassDecl(M6526Window);
public:
    /// constructor
    M6526Window(const char* title, m6526_t* cia);
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

    const char* Name = nullptr;
    m6526_t* CIA = nullptr;
};

} // namespace YAKC