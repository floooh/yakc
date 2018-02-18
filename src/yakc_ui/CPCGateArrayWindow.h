#pragma once
//------------------------------------------------------------------------------
/**
    @class CPCGateArrayWindow
    @brief CPC gate array debugging UI
*/
#include "yakc_ui/WindowBase.h"
#include "IMUI/IMUI.h"

namespace YAKC {

class CPCGateArrayWindow : public WindowBase {
    OryolClassDecl(CPCGateArrayWindow);
public:
    virtual void Setup(yakc& emu) override;
    virtual bool Draw(yakc& emu) override;

    void drawMainWindow(yakc& emu);
    void drawColorWindow(yakc& emu);

    bool colorWindowOpen = false;
    ImVec4 hwColors[32];
    ImVec4 paletteColors[16];
    ImVec4 borderColor;
};

} // namespace YAKC
