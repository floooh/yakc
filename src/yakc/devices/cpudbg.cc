//------------------------------------------------------------------------------
//  cpudbg.cc
//------------------------------------------------------------------------------
#include "cpudbg.h"

namespace YAKC {

//------------------------------------------------------------------------------
cpudbg::cpudbg() :
pc_history_pos(0),
paused(false) {
    memset(&this->pc_history, 0, sizeof(this->pc_history));
}

//------------------------------------------------------------------------------
bool
cpudbg::check_break(uint16_t pc) const {
    for (int i = 0; i < max_breakpoints; i++) {
        if (this->breakpoints[i].enabled) {
            if (pc == this->breakpoints[i].address) {
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
void
cpudbg::store_pc_history(uint16_t pc) {
    this->pc_history[pc_history_pos++] = pc;
    this->pc_history_pos &= this->pc_history_size-1;
}

//------------------------------------------------------------------------------
uint16_t
cpudbg::get_pc_history(int index) const {
    int i = ((this->pc_history_pos-pc_history_size)+index) & (pc_history_size-1);
    return pc_history[i];
}

//------------------------------------------------------------------------------
void
cpudbg::enable_breakpoint(int index, uint16_t addr) {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    this->breakpoints[index].enabled = true;
    this->breakpoints[index].address = addr;
}

//------------------------------------------------------------------------------
void
cpudbg::disable_breakpoint(int index) {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    this->breakpoints[index].enabled = false;
}

//------------------------------------------------------------------------------
void
cpudbg::toggle_breakpoint(int index, uint16_t addr) {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    if (this->breakpoints[index].address == addr) {
        this->breakpoints[index].enabled = !this->breakpoints[index].enabled;
    }
    else {
        this->enable_breakpoint(index, addr);
    }
}

//------------------------------------------------------------------------------
bool
cpudbg::is_breakpoint(uint16_t addr) const {
    for (int i = 0; i < max_breakpoints; i++) {
        if ((this->breakpoints[i].enabled) && (this->breakpoints[i].address == addr)) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool
cpudbg::breakpoint_enabled(int index) const {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    return this->breakpoints[index].enabled;
}

//------------------------------------------------------------------------------
uint16_t
cpudbg::breakpoint_addr(int index) const {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    return this->breakpoints[index].address;
}

//------------------------------------------------------------------------------
void
cpudbg::step_pc_modified(system_bus* bus, z80& cpu) {
    YAKC_ASSERT(bus);
    uint16_t pc;
    do {
        pc = cpu.PC;
        this->store_pc_history(pc);
        cpu.step(bus);
    }
    while ((pc == cpu.PC) && !cpu.INV);
}

//------------------------------------------------------------------------------
void
cpudbg::step_pc_modified(mos6502& cpu) {
    uint16_t pc;
    do {
        pc = cpu.PC;
        this->store_pc_history(pc);
        cpu.step();
    }
    while (pc == cpu.PC);
}

//------------------------------------------------------------------------------
void
cpudbg::set8(z80& cpu, z80reg r, uint8_t v) {
    switch (r) {
        case z80reg::A:     cpu.A = v; break;
        case z80reg::F:     cpu.F = v; break;
        case z80reg::B:     cpu.B = v; break;
        case z80reg::C:     cpu.C = v; break;
        case z80reg::D:     cpu.D = v; break;
        case z80reg::E:     cpu.E = v; break;
        case z80reg::H:     cpu.H = v; break;
        case z80reg::L:     cpu.L = v; break;
        case z80reg::I:     cpu.I = v; break;
        case z80reg::R:     cpu.R = v; break;
        case z80reg::IM:    cpu.IM = v; break;
        default:break;
    }
}

//------------------------------------------------------------------------------
uint8_t
cpudbg::get8(const z80& cpu, z80reg r) {
    switch (r) {
        case z80reg::A:    return cpu.A;
        case z80reg::F:    return cpu.F;
        case z80reg::B:    return cpu.B;
        case z80reg::C:    return cpu.C;
        case z80reg::D:    return cpu.D;
        case z80reg::E:    return cpu.E;
        case z80reg::H:    return cpu.H;
        case z80reg::L:    return cpu.L;
        case z80reg::I:    return cpu.I;
        case z80reg::R:    return cpu.R;
        case z80reg::IM:   return cpu.IM;
        default: return 0;
    }
}

//------------------------------------------------------------------------------
void
cpudbg::set16(z80& cpu, z80reg r, uint16_t v) {
    switch (r) {
        case z80reg::AF:   cpu.AF = v; break;
        case z80reg::BC:   cpu.BC = v; break;
        case z80reg::DE:   cpu.DE = v; break;
        case z80reg::HL:   cpu.HL = v; break;
        case z80reg::WZ:   cpu.WZ = v; break;
        case z80reg::AF_:  cpu.AF_ = v; break;
        case z80reg::BC_:  cpu.BC_ = v; break;
        case z80reg::DE_:  cpu.DE_ = v; break;
        case z80reg::HL_:  cpu.HL_ = v; break;
        case z80reg::WZ_:  cpu.WZ_ = v; break;
        case z80reg::IX:   cpu.IX = v; break;
        case z80reg::IY:   cpu.IY = v; break;
        case z80reg::SP:   cpu.SP = v; break;
        case z80reg::PC:   cpu.PC = v; break;
        default: break;
    }
}

//------------------------------------------------------------------------------
uint16_t
cpudbg::get16(const z80& cpu, z80reg r) {
    switch (r) {
        case z80reg::AF:    return cpu.AF;
        case z80reg::BC:    return cpu.BC;
        case z80reg::DE:    return cpu.DE;
        case z80reg::HL:    return cpu.HL;
        case z80reg::WZ:    return cpu.WZ;
        case z80reg::AF_:   return cpu.AF_;
        case z80reg::BC_:   return cpu.BC_;
        case z80reg::DE_:   return cpu.DE_;
        case z80reg::HL_:   return cpu.HL_;
        case z80reg::WZ_:   return cpu.WZ_;
        case z80reg::IX:    return cpu.IX;
        case z80reg::IY:    return cpu.IY;
        case z80reg::SP:    return cpu.SP;
        case z80reg::PC:    return cpu.PC;
        default: return 0;
    }
}

//------------------------------------------------------------------------------
const char*
cpudbg::reg_name(z80reg r) {
    switch (r) {
        case z80reg::A: return "A";
        case z80reg::F: return "F";
        case z80reg::B: return "B";
        case z80reg::C: return "C";
        case z80reg::D: return "D";
        case z80reg::E: return "E";
        case z80reg::H: return "H";
        case z80reg::L: return "L";
        case z80reg::AF: return "AF";
        case z80reg::BC: return "BC";
        case z80reg::DE: return "DE";
        case z80reg::HL: return "HL";
        case z80reg::WZ: return "WZ";
        case z80reg::AF_: return "AF'";
        case z80reg::BC_: return "BC'";
        case z80reg::DE_: return "DE'";
        case z80reg::HL_: return "HL'";
        case z80reg::WZ_: return "WZ'";
        case z80reg::I: return "I";
        case z80reg::R: return "R";
        case z80reg::IX: return "IX";
        case z80reg::IY: return "IY";
        case z80reg::SP: return "SP";
        case z80reg::PC: return "PC";
        case z80reg::IM: return "IM";
        default: return "?";
    }
}

//------------------------------------------------------------------------------
void
cpudbg::set8(mos6502& cpu, m6502reg r, uint8_t v) {
    switch (r) {
        case m6502reg::A:   cpu.A = v; break;
        case m6502reg::X:   cpu.X = v; break;
        case m6502reg::Y:   cpu.Y = v; break;
        case m6502reg::S:   cpu.S = v; break;
        case m6502reg::P:   cpu.P = v; break;
        default: break;
    }
}

//------------------------------------------------------------------------------
uint8_t
cpudbg::get8(const mos6502& cpu, m6502reg r) {
    switch (r) {
        case m6502reg::A:   return cpu.A;
        case m6502reg::X:   return cpu.X;
        case m6502reg::Y:   return cpu.Y;
        case m6502reg::S:   return cpu.S;
        case m6502reg::P:   return cpu.P;
        default: return 0;
    }
}

//------------------------------------------------------------------------------
void
cpudbg::set16(mos6502& cpu, m6502reg r, uint16_t v) {
    if (r == m6502reg::PC) {
        cpu.PC = v;
    }
}

//------------------------------------------------------------------------------
uint16_t
cpudbg::get16(const mos6502& cpu, m6502reg r) {
    if (r == m6502reg::PC) {
        return cpu.PC;
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
const char*
cpudbg::reg_name(m6502reg r) {
    switch (r) {
        case m6502reg::A: return "A";
        case m6502reg::X: return "X";
        case m6502reg::Y: return "Y";
        case m6502reg::S: return "S";
        case m6502reg::P: return "P";
        case m6502reg::PC: return "PC";
        default: return "?";
    }
}

} // namespace YAKC
