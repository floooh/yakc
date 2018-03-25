#pragma once
//------------------------------------------------------------------------------
/**
    @class C64Window.h
    @brief general C64 debug info
*/
#include "yakc_ui/WindowBase.h"

namespace YAKC {

class C64Window : public WindowBase {
    OryolClassDecl(C64Window);
public:
    virtual void Setup(yakc& emu) override;
    virtual bool Draw(yakc& emu) override;
};

} // namespace YAKC
