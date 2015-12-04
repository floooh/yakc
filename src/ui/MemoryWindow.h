#pragma once
//------------------------------------------------------------------------------
/**
    @class MemoryWindow
    @brief memory dump and edit window
*/
#include "ui/WindowBase.h"
#include "ui/ImGuiMemoryEditor.h"

class MemoryWindow : public WindowBase {
    OryolClassDecl(MemoryWindow);
public:
    /// setup the window
    virtual void Setup(yakc::kc85& kc) override;
    /// draw method
    virtual bool Draw(yakc::kc85& kc) override;

    MemoryEditor edit;
};