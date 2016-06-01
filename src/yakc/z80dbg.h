#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z80dbg
    @brief debug helper functions
*/
#include "yakc/core.h"
#include "yakc/z80.h"

namespace YAKC {

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
    /// execution paused (e.g. because in debugger)
    bool paused;

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
    /// return true if breakpoint is enabled
    bool breakpoint_enabled(int index) const;
    /// get breakpoint address
    uword breakpoint_addr(int index) const;

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
    struct breakpoint {
        breakpoint() : enabled(false), address(0) {};
        bool enabled;
        uword address;
    } breakpoints[max_breakpoints];
};

//------------------------------------------------------------------------------
inline
z80dbg::z80dbg() :
pc_history_pos(0),
paused(false) {
    memset(&this->pc_history, 0, sizeof(this->pc_history));
}

//------------------------------------------------------------------------------
inline bool
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
inline void
z80dbg::store_pc_history(const z80& cpu) {
    this->pc_history[pc_history_pos++] = cpu.PC;
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
inline bool
z80dbg::breakpoint_enabled(int index) const {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    return this->breakpoints[index].enabled;
}

//------------------------------------------------------------------------------
inline uword
z80dbg::breakpoint_addr(int index) const {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    return this->breakpoints[index].address;
}

//------------------------------------------------------------------------------
inline void
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
inline void
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
inline ubyte
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
inline void
z80dbg::set16(z80& cpu, reg r, uword v) {
    switch (r) {
        case AF:   cpu.AF = v; break;
        case BC:   cpu.BC = v; break;
        case DE:   cpu.DE = v; break;
        case HL:   cpu.HL = v; break;
        case AF_:  cpu.AF_ = v; break;
        case BC_:  cpu.BC_ = v; break;
        case DE_:  cpu.DE_ = v; break;
        case HL_:  cpu.HL_ = v; break;
        case IX:   cpu.IX = v; break;
        case IY:   cpu.IY = v; break;
        case SP:   cpu.SP = v; break;
        case PC:   cpu.PC = v; break;
        default:   YAKC_ASSERT(false);
    }
}

//------------------------------------------------------------------------------
inline uword
z80dbg::get16(const z80& cpu, reg r) {
    switch (r) {
        case AF:    return cpu.AF;
        case BC:    return cpu.BC;
        case DE:    return cpu.DE;
        case HL:    return cpu.HL;
        case AF_:   return cpu.AF_;
        case BC_:   return cpu.BC_;
        case DE_:   return cpu.DE_;
        case HL_:   return cpu.HL_;
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

} // namespace YAKC
