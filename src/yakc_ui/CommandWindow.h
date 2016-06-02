#pragma once
//------------------------------------------------------------------------------
/**
    @class CommandWindow
    @brief scan memory for commands
*/
#include "yakc_ui/WindowBase.h"
#include "yakc_ui/HexInputWidget.h"
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
    void scan(const yakc& emu, ubyte prologByte);

    HexInputWidget prologByteWidget;
    struct Cmd {
        Cmd(const Oryol::String& n, uword a) : name(n), addr(a) {};
        Oryol::String name;
        uword addr;
    };
    Oryol::Array<Cmd> commands;
};

} // namespace YAKC


