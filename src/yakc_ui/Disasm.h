#pragma once
//------------------------------------------------------------------------------
/**
    @class Disasm
    @brief YAKC wrapper for z80dasm.cc
*/
#include "yakc/yakc.h"

namespace YAKC {

class Disasm {
public:
    /// constructor
    Disasm();
    /// disassemble instruction at addr, return new addr
    uword Disassemble(const yakc& emu, uword addr);
    /// get disassembled string
    const char* Result() const;

private:
    /// fetch next opcode byte for z80dasm
    static ubyte fetch(uword base, int offset, void* userdata);

    const yakc* emu;
    char buffer[64];
};

} // namespace YAKC
