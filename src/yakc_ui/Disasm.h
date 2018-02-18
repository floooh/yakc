#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::Disasm
    @brief YAKC wrapper for z80dasm.cc
*/
#include "yakc/yakc.h"

namespace YAKC {

class Disasm {
public:
    /// constructor
    Disasm();
    /// disassemble instruction at addr, return new addr
    uint16_t Disassemble(const yakc& emu, uint16_t addr);
    /// get disassembled string
    const char* Result() const;

private:
    const yakc* emu = nullptr;
    char buffer[64];
};

} // namespace YAKC
