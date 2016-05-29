#pragma once
//------------------------------------------------------------------------------
/**
    @class MemoryWindow
    @brief memory dump and edit window
*/
#include "ui/WindowBase.h"
#include "ui/ImGuiMemoryEditor.h"

namespace yakc {

class MemoryWindow : public WindowBase {
    OryolClassDecl(MemoryWindow);
public:
    /// setup the window
    virtual void Setup(yakc::emu& emu) override;
    /// draw method
    virtual bool Draw(yakc::emu& emu) override;

    MemoryEditor edit;
};

} // namespace yakc
