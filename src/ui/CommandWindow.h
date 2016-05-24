#pragma once
//------------------------------------------------------------------------------
/**
    @class CommandWindow
    @brief scan memory for commands
*/
#include "ui/HexInputWidget.h"
#include "ui/WindowBase.h"

class CommandWindow : public WindowBase {
    OryolClassDecl(CommandWindow);
public:
    /// setup the window
    virtual void Setup(yakc::emu& emu) override;
    /// draw method
    virtual bool Draw(yakc::emu& emu) override;

    /// populate commands array
    void scan(const yakc::emu& emu, yakc::ubyte prologByte);

    HexInputWidget prologByteWidget;
    struct Cmd {
        Cmd(const Oryol::String& n, yakc::uword a) : name(n), addr(a) {};
        Oryol::String name;
        yakc::uword addr;
    };
    Oryol::Array<Cmd> commands;
};

