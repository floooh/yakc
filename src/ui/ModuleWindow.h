#pragma once
//------------------------------------------------------------------------------
/**
    @class ModuleWindow
    @brief allows to attach or remove expansion modules to KC device
*/
#include "yakc/KC85Oryol.h"
#include "ui/WindowBase.h"
#include "Core/Containers/Array.h"

namespace yakc {

class ModuleWindow : public WindowBase {
    OryolClassDecl(ModuleWindow);
public:
    /// setup the window
    virtual void Setup(yakc::emu& emu) override;
    /// draw method
    virtual bool Draw(yakc::emu& emu) override;
    /// draw a single module slot
    void drawModuleSlot(yakc::kc85& kc, yakc::ubyte slot_addr);
};

} // namespace yakc

