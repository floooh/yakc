#pragma once
//------------------------------------------------------------------------------
/**
    @class MemoryWindow
    @brief memory dump and edit window
*/
#include "yakc_ui/WindowBase.h"
#include "yakc_ui/ImGuiMemoryEditor.h"

class MemoryWindow : public WindowBase {
    OryolClassDecl(MemoryWindow);
public:
    /// configure the memory bank (0..3) before setup() is called
    int MemoryBankIndex = 0;

    /// setup the window
    virtual void Setup(const yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

    MemoryEditor edit;
};