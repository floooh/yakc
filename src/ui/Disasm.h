#pragma once
//------------------------------------------------------------------------------
/**
    @class Disasm
    @brief YAKC wrapper for z80dasm.cc
*/
#include "yakc/KC85Oryol.h"

namespace yakc {

class Disasm {
public:
    /// constructor
    Disasm();
    /// disassemble instruction at addr, return new addr
    yakc::uword Disassemble(const yakc::emu& emu, yakc::uword addr);
    /// get disassembled string
    const char* Result() const;

private:
    /// fetch next opcode byte for z80dasm
    static yakc::ubyte fetch(yakc::uword base, int offset, void* userdata);

    const yakc::emu* context;
    char buffer[64];
};

} // namespace yakc
