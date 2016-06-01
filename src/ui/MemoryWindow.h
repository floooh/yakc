#pragma once
//------------------------------------------------------------------------------
/**
    @class MemoryWindow
    @brief memory dump and edit window
*/
#include "ui/WindowBase.h"
#include "ui/ImGuiMemoryEditor.h"

namespace YAKC {

class MemoryWindow : public WindowBase {
    OryolClassDecl(MemoryWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

    MemoryEditor edit;
};

} // namespace YAKC
