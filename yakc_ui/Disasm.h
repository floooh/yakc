#pragma once
//------------------------------------------------------------------------------
/**
    @class Disasm
    @brief YAKC wrapper for z80dasm.cc
*/
#include "yakc_app/KC85Oryol.h"

class Disasm {
public:
    /// constructor
    Disasm();
    /// disassemble instruction at addr, return new addr
    yakc::uword Disassemble(const yakc::kc85& kc, yakc::uword addr);
    /// get disassembled string
    const char* Result() const;

private:
    /// fetch next opcode byte for z80dasm
    static yakc::ubyte fetch(yakc::uword base, int offset, void* userdata);

    const yakc::kc85* context;
    char buffer[64];
};
