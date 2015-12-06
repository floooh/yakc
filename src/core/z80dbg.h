#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z80dbg
    @brief debug helper functions
*/
#include "core/common.h"
#include "core/z80.h"

namespace yakc {

class z80dbg {
public:
    /// register enum for set_reg/get_reg methods
    enum reg {
        A=0, F, B, C, D, E, H, L,
        AF, BC, DE, HL,
        AF_, BC_, DE_, HL_,
        I, R, IX, IY, SP, PC,
        IM,
        num
    };

    /// size of PC history ringbuffer (must be 2^N!)
    static const int pc_history_size = 8;
    /// current pc history position
    int pc_history_pos;
    /// a history ringbuffer of previous PC addresses
    uword pc_history[pc_history_size];

    /// constructor
    z80dbg();

    /// test whether breakpoint is enabled and hit
    bool check_break(const z80& cpu) const;
    /// store current pc in history ringbuffer
    void store_pc_history(const z80& cpu);
    /// get pc from history ringbuffer (0 is oldest entry)
    uword get_pc_history(int index) const;

    /// enable a breakpoint
    void enable_breakpoint(int index, uword addr);
    /// disable a breakpoint
    void disable_breakpoint(int index);
    /// if breakpoint at addr exists, toggle state, else enable breakpoint
    void toggle_breakpoint(int index, uword addr);
    /// test if an address is a breakpoint
    bool is_breakpoint(uword addr) const;

    /// step until PC changed (or an invalid opcode is hit)
    void step_pc_modified(z80& cpu);

    /// set an 8-bit register value by enum (slow)
    static void set8(z80& cpu, reg r, ubyte v);
    /// get an 8-bit register value by enum (slow)
    static ubyte get8(const z80& cpu, reg r);
    /// set a 16-bit register value by enum (slow)
    static void set16(z80& cpu, reg r, uword v);
    /// get a 16-bit register value by enum (slow)
    static uword get16(const z80& cpu, reg r);
    /// get a string-name for a register
    static const char* reg_name(reg r);

private:
    static const int max_breakpoints = 2;
    struct {
        bool enabled = false;
        uword address = 0;
    } breakpoints[max_breakpoints];
};

//------------------------------------------------------------------------------
inline
z80dbg::z80dbg() :
pc_history_pos(0) {
    YAKC_MEMSET(&this->pc_history, 0, sizeof(this->pc_history));
}

//------------------------------------------------------------------------------
inline bool
z80dbg::check_break(const z80& cpu) const {
    for (int i = 0; i < max_breakpoints; i++) {
        if (this->breakpoints[i].enabled) {
            if (cpu.state.PC == this->breakpoints[i].address) {
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
inline void
z80dbg::store_pc_history(const z80& cpu) {
    this->pc_history[pc_history_pos++] = cpu.state.PC;
    this->pc_history_pos &= this->pc_history_size-1;
}

//------------------------------------------------------------------------------
inline uword
z80dbg::get_pc_history(int index) const {
    int i = ((this->pc_history_pos-pc_history_size)+index) & (pc_history_size-1);
    return pc_history[i];
}

//------------------------------------------------------------------------------
inline void
z80dbg::enable_breakpoint(int index, uword addr) {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    this->breakpoints[index].enabled = true;
    this->breakpoints[index].address = addr;
}

//------------------------------------------------------------------------------
inline void
z80dbg::disable_breakpoint(int index) {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    this->breakpoints[index].enabled = false;
}

//------------------------------------------------------------------------------
inline void
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
inline bool
z80dbg::is_breakpoint(uword addr) const {
    for (int i = 0; i < max_breakpoints; i++) {
        if ((this->breakpoints[i].enabled) && (this->breakpoints[i].address == addr)) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
inline void
z80dbg::step_pc_modified(z80& cpu) {
    uword pc;
    do {
        pc = cpu.state.PC;
        this->store_pc_history(cpu);
        cpu.step();
    }
    while ((pc == cpu.state.PC) && !cpu.state.INV);
}

//------------------------------------------------------------------------------
inline void
z80dbg::set8(z80& cpu, reg r, ubyte v) {
    switch (r) {
        case A:     cpu.state.A = v; break;
        case F:     cpu.state.F = v; break;
        case B:     cpu.state.B = v; break;
        case C:     cpu.state.C = v; break;
        case D:     cpu.state.D = v; break;
        case E:     cpu.state.E = v; break;
        case H:     cpu.state.H = v; break;
        case L:     cpu.state.L = v; break;
        case I:     cpu.state.I = v; break;
        case R:     cpu.state.R = v; break;
        case IM:    cpu.state.IM = v; break;
        default:    YAKC_ASSERT(false); break;
    }
}

//------------------------------------------------------------------------------
inline ubyte
z80dbg::get8(const z80& cpu, reg r) {
    switch (r) {
        case A:    return cpu.state.A;
        case F:    return cpu.state.F;
        case B:    return cpu.state.B;
        case C:    return cpu.state.C;
        case D:    return cpu.state.D;
        case E:    return cpu.state.E;
        case H:    return cpu.state.H;
        case L:    return cpu.state.L;
        case I:    return cpu.state.I;
        case R:    return cpu.state.R;
        case IM:   return cpu.state.IM;
        default:
            YAKC_ASSERT(false);
            return 0;
    }
}

//------------------------------------------------------------------------------
inline void
z80dbg::set16(z80& cpu, reg r, uword v) {
    switch (r) {
        case AF:   cpu.state.AF = v; break;
        case BC:   cpu.state.BC = v; break;
        case DE:   cpu.state.DE = v; break;
        case HL:   cpu.state.HL = v; break;
        case AF_:  cpu.state.AF_ = v; break;
        case BC_:  cpu.state.BC_ = v; break;
        case DE_:  cpu.state.DE_ = v; break;
        case HL_:  cpu.state.HL_ = v; break;
        case IX:   cpu.state.IX = v; break;
        case IY:   cpu.state.IY = v; break;
        case SP:   cpu.state.SP = v; break;
        case PC:   cpu.state.PC = v; break;
        default:   YAKC_ASSERT(false);
    }
}

//------------------------------------------------------------------------------
inline uword
z80dbg::get16(const z80& cpu, reg r) {
    switch (r) {
        case AF:    return cpu.state.AF;
        case BC:    return cpu.state.BC;
        case DE:    return cpu.state.DE;
        case HL:    return cpu.state.HL;
        case AF_:   return cpu.state.AF_;
        case BC_:   return cpu.state.BC_;
        case DE_:   return cpu.state.DE_;
        case HL_:   return cpu.state.HL_;
        case IX:    return cpu.state.IX;
        case IY:    return cpu.state.IY;
        case SP:    return cpu.state.SP;
        case PC:    return cpu.state.PC;
        default:
            YAKC_ASSERT(false);
            return 0;
    }
}

//------------------------------------------------------------------------------
inline const char*
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
        case AF_: return "AF'";
        case BC_: return "BC'";
        case DE_: return "DE'";
        case HL_: return "HL'";
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

} // namespace yakc
