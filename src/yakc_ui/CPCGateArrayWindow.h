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

    void drawColors();

    ImVec4 paletteColors[16];
    ImVec4 borderColor;
};

} // namespace YAKC
