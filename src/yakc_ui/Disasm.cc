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
uint8_t
Disasm::fetch(uint16_t base, int offset, void* /*userdata*/) {
    return mem_rd(&board.mem, base+offset);
}

//------------------------------------------------------------------------------
uint16_t
Disasm::Disassemble(const yakc& emu, uint16_t addr) {
    int res = 0;
    this->emu = &emu;
    if (this->emu->cpu_type() == cpu_model::mos6502) {
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
