#pragma once
//------------------------------------------------------------------------------
/**
    @class CommandWindow
    @brief scan memory for commands
*/
#include "yakc_ui/WindowBase.h"
#include "Core/Containers/Array.h"

namespace YAKC {

class CommandWindow : public WindowBase {
    OryolClassDecl(CommandWindow);
public:
    /// setup the window
    virtual void Setup(yakc& emu) override;
    /// draw method
    virtual bool Draw(yakc& emu) override;

    /// populate commands array
    void scan(const yakc& emu, uint8_t prologByte);

    uint8_t prologByte = 0x7F;
    struct Cmd {
        Cmd(const Oryol::String& n, uint16_t a) : name(n), addr(a) {};
        Oryol::String name;
        uint16_t addr;
    };
    Oryol::Array<Cmd> commands;
};

} // namespace YAKC


