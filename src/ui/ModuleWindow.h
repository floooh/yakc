#pragma once
//------------------------------------------------------------------------------
/**
    @class ModuleWindow
    @brief allows to attach or remove expansion modules to KC device
*/
#include "yakcapp/KC85Oryol.h"
#include "ui/WindowBase.h"
#include "Core/Containers/Array.h"

namespace YAKC {

class ModuleWindow : public WindowBase {
    OryolClassDecl(ModuleWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;
    /// draw a single module slot
    void drawModuleSlot(kc85& kc, ubyte slot_addr);
};

} // namespace YAKC

