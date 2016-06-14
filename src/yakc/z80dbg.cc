//------------------------------------------------------------------------------
//  z80dbg.cc
//------------------------------------------------------------------------------
#include "z80dbg.h"

namespace YAKC {

//------------------------------------------------------------------------------
z80dbg::z80dbg() :
pc_history_pos(0),
paused(false) {
    memset(&this->pc_history, 0, sizeof(this->pc_history));
}

//------------------------------------------------------------------------------
bool
z80dbg::check_break(const z80& cpu) const {
    for (int i = 0; i < max_breakpoints; i++) {
        if (this->breakpoints[i].enabled) {
            if (cpu.PC == this->breakpoints[i].address) {
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
void
z80dbg::store_pc_history(const z80& cpu) {
    this->pc_history[pc_history_pos++] = cpu.PC;
    this->pc_history_pos &= this->pc_history_size-1;
}

//------------------------------------------------------------------------------
uword
z80dbg::get_pc_history(int index) const {
    int i = ((this->pc_history_pos-pc_history_size)+index) & (pc_history_size-1);
    return pc_history[i];
}

//------------------------------------------------------------------------------
void
z80dbg::enable_breakpoint(int index, uword addr) {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    this->breakpoints[index].enabled = true;
    this->breakpoints[index].address = addr;
}

//------------------------------------------------------------------------------
void
z80dbg::disable_breakpoint(int index) {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    this->breakpoints[index].enabled = false;
}

//------------------------------------------------------------------------------
void
z80dbg::toggle_breakpoint(int index, uword addr) {
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
z80dbg::is_breakpoint(uword addr) const {
    for (int i = 0; i < max_breakpoints; i++) {
        if ((this->breakpoints[i].enabled) && (this->breakpoints[i].address == addr)) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool
z80dbg::breakpoint_enabled(int index) const {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    return this->breakpoints[index].enabled;
}

//------------------------------------------------------------------------------
uword
z80dbg::breakpoint_addr(int index) const {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    return this->breakpoints[index].address;
}

//------------------------------------------------------------------------------
void
z80dbg::step_pc_modified(z80& cpu) {
    uword pc;
    do {
        pc = cpu.PC;
        this->store_pc_history(cpu);
        cpu.step();
    }
    while ((pc == cpu.PC) && !cpu.INV);
}

//------------------------------------------------------------------------------
void
z80dbg::set8(z80& cpu, reg r, ubyte v) {
    switch (r) {
        case A:     cpu.A = v; break;
        case F:     cpu.F = v; break;
        case B:     cpu.B = v; break;
        case C:     cpu.C = v; break;
        case D:     cpu.D = v; break;
        case E:     cpu.E = v; break;
        case H:     cpu.H = v; break;
        case L:     cpu.L = v; break;
        case I:     cpu.I = v; break;
        case R:     cpu.R = v; break;
        case IM:    cpu.IM = v; break;
        default:    YAKC_ASSERT(false); break;
    }
}

//------------------------------------------------------------------------------
ubyte
z80dbg::get8(const z80& cpu, reg r) {
    switch (r) {
        case A:    return cpu.A;
        case F:    return cpu.F;
        case B:    return cpu.B;
        case C:    return cpu.C;
        case D:    return cpu.D;
        case E:    return cpu.E;
        case H:    return cpu.H;
        case L:    return cpu.L;
        case I:    return cpu.I;
        case R:    return cpu.R;
        case IM:   return cpu.IM;
        default:
            YAKC_ASSERT(false);
            return 0;
    }
}

//------------------------------------------------------------------------------
void
z80dbg::set16(z80& cpu, reg r, uword v) {
    switch (r) {
        case AF:   cpu.AF = v; break;
        case BC:   cpu.BC = v; break;
        case DE:   cpu.DE = v; break;
        case HL:   cpu.HL = v; break;
        case WZ:   cpu.WZ = v; break;
        case AF_:  cpu.AF_ = v; break;
        case BC_:  cpu.BC_ = v; break;
        case DE_:  cpu.DE_ = v; break;
        case HL_:  cpu.HL_ = v; break;
        case WZ_:  cpu.WZ_ = v; break;
        case IX:   cpu.IX = v; break;
        case IY:   cpu.IY = v; break;
        case SP:   cpu.SP = v; break;
        case PC:   cpu.PC = v; break;
        default:   YAKC_ASSERT(false);
    }
}

//------------------------------------------------------------------------------
uword
z80dbg::get16(const z80& cpu, reg r) {
    switch (r) {
        case AF:    return cpu.AF;
        case BC:    return cpu.BC;
        case DE:    return cpu.DE;
        case HL:    return cpu.HL;
        case WZ:    return cpu.WZ;
        case AF_:   return cpu.AF_;
        case BC_:   return cpu.BC_;
        case DE_:   return cpu.DE_;
        case HL_:   return cpu.HL_;
        case WZ_:   return cpu.WZ_;
        case IX:    return cpu.IX;
        case IY:    return cpu.IY;
        case SP:    return cpu.SP;
        case PC:    return cpu.PC;
        default:
            YAKC_ASSERT(false);
            return 0;
    }
}

//------------------------------------------------------------------------------
const char*
z80dbg::reg_name(reg r) {
    switch (r) {
        case A: return "A";
        case F: return "F";
        case B: return "B";
        case C: return "C";
        case D: return "D";
        case E: return "E";
        case H: return "H";
        case L: return "L";
        case AF: return "AF";
        case BC: return "BC";
        case DE: return "DE";
        case HL: return "HL";
        case WZ: return "WZ";
        case AF_: return "AF'";
        case BC_: return "BC'";
        case DE_: return "DE'";
        case HL_: return "HL'";
        case WZ_: return "WZ'";
        case I: return "I";
        case R: return "R";
        case IX: return "IX";
        case IY: return "IY";
        case SP: return "SP";
        case PC: return "PC";
        case IM: return "IM";
        default: return "?";
    }
}

} // namespace YAKC
