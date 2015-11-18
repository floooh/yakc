#pragma once
//------------------------------------------------------------------------------
/**
    @class ModuleWindow
    @brief allows to attach or remove expansion modules to KC device
*/
#include "yakc_app/KC85Oryol.H"
#include "yakc_ui/WindowBase.h"
#include "Core/Containers/Array.h"

class ModuleWindow : public WindowBase {
    OryolClassDecl(ModuleWindow);
public:
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;
private:
    /// setup the list of supported modules
    void setupModules(yakc::kc85& kc);
    /// draw a single module slot
    void drawModuleSlot(yakc::kc85& kc, yakc::ubyte slot_addr);

    Oryol::Array<yakc::kc85_module> modules;
};