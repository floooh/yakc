//------------------------------------------------------------------------------
//  Disasm.cc
//------------------------------------------------------------------------------
#include "Disasm.h"
#include "z80dasm.h"
#include "mos6502dasm.h"
#include "Core/Memory/Memory.h"

using namespace Oryol;
using namespace z80dasm;
using namespace mos6502dasm;

namespace YAKC {

//------------------------------------------------------------------------------
Disasm::Disasm() :
emu(0) {
    Memory::Clear(this->buffer, sizeof(this->buffer));
}

//------------------------------------------------------------------------------
ubyte
Disasm::fetch(uword base, int offset, void* userdata) {
    Disasm* self = (Disasm*) userdata;
    if (self->emu->cpu_model() == cpu_model::mos6502) {
        return self->emu->board.mos6502.mem.r8io(base + offset);
    }
    else {
        return self->emu->board.z80.mem.r8(base + offset);
    }
}

//------------------------------------------------------------------------------
uword
Disasm::Disassemble(const yakc& emu, uword addr) {
    int res = 0;
    this->emu = &emu;
    if (this->emu->cpu_model() == cpu_model::mos6502) {
        res = mos6502disasm(fetch, addr, this->buffer, this);
    }
    else {
        res = z80disasm(fetch, addr, this->buffer, this);
    }
    return (res & 0xFFFF);
}

//------------------------------------------------------------------------------
const char*
Disasm::Result() const {
    return this->buffer;
}

} // namespace YAKC
