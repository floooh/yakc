#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z80
    @brief the Z80 (or rather U880) cpu
*/
#include <string.h>
#include "yakc_core/common.h"
#include "yakc_core/memory.h"

namespace yakc {

class z80 {
public:

    /// flag bits
    enum flag {
        CF = (1<<0),        // carry flag
        NF = (1<<1),        // add/subtract
        VF = (1<<2),        // parity/overflow
        PF = VF,            // parity/overflow
        XF = (1<<3),        // undocumented bit 3
        HF = (1<<4),        // half-carry
        YF = (1<<5),        // undocumented bit 5
        ZF = (1<<6),        // zero flag
        SF = (1<<7),        // sign flag
    };
    /// register enum for set_reg/get_reg methods
    enum reg {
        A=0, F, B, C, D, E, H, L,
        AF, BC, DE, HL,
        AF_,BC_, DE_, HL_,
        I, R, IX, IY, SP, PC,
        IM,
        num
    };

    /// the cpu state
    struct cpu_state {
        /// main register set
        union {
            struct { ubyte F, A; };
            uword AF;
        };
        union {
            struct { ubyte C, B; };
            uword BC;
        };
        union {
            struct { ubyte E, D; };
            uword DE;
        };
        union {
            struct { ubyte L, H; };
            uword HL;
        };

        /// shadow register set
        uword AF_;
        uword BC_;
        uword DE_;
        uword HL_;

        /// special registers
        ubyte I;
        ubyte R;
        uword IX;
        uword IY;
        uword SP;
        uword PC;

        /// CPU is in HALT state
        bool HALT;
        /// the interrupt-enable flip-flops
        bool IFF1;
        bool IFF2;
        /// the interrupt mode (0, 1 or 2)
        ubyte IM;
        /// current T cycle counter
        unsigned int T;
    } state;

    // memory map
    memory mem;

    /// constructor
    z80() {
        memset(&this->state, 0, sizeof(this->state));
    }
    /// helper method to swap 2 16-bit registers
    static void swap16(uword& r0, uword& r1) {
        uword tmp = r0;
        r0 = r1;
        r1 = tmp;
    }
    /// set an 8-bit register value by enum (slow)
    void set8(reg r, ubyte v) {
        switch (r) {
            case A:     state.A = v; break;
            case F:     state.F = v; break;
            case B:     state.B = v; break;
            case C:     state.C = v; break;
            case D:     state.D = v; break;
            case E:     state.E = v; break;
            case H:     state.H = v; break;
            case L:     state.L = v; break;
            case I:     state.I = v; break;
            case R:     state.R = v; break;
            case IM:    state.IM = v; break;
            default:    YAKC_ASSERT(false); break;
        }
    }
    /// get an 8-bit register value by enum (slow)
    ubyte get8(reg r) const {
        switch (r) {
            case A:     return state.A;
            case F:     return state.F;
            case B:     return state.B;
            case C:     return state.C;
            case D:     return state.D;
            case E:     return state.E;
            case H:     return state.H;
            case L:     return state.L;
            case I:     return state.I;
            case R:     return state.R;
            case IM:    return state.IM;
            default:
                YAKC_ASSERT(false);
                return 0;
        }
    }
    /// set a 16-bit register value by enum (slow)
    void set16(reg r, uword v) {
        switch (r) {
            case AF:    state.AF = v; break;
            case BC:    state.BC = v; break;
            case DE:    state.DE = v; break;
            case HL:    state.HL = v; break;
            case AF_:   state.AF_ = v; break;
            case BC_:   state.BC_ = v; break;
            case DE_:   state.DE_ = v; break;
            case HL_:   state.HL_ = v; break;
            case IX:    state.IX = v; break;
            case IY:    state.IY = v; break;
            case SP:    state.SP = v; break;
            case PC:    state.PC = v; break;
            default:    YAKC_ASSERT(false);
        }
    }
    /// get a 16-bit register value by enum (slow)
    uword get16(reg r) const {
        switch (r) {
            case AF:    return state.AF;
            case BC:    return state.BC;
            case DE:    return state.DE;
            case HL:    return state.HL;
            case AF_:   return state.AF_;
            case BC_:   return state.BC_;
            case DE_:   return state.DE_;
            case HL_:   return state.HL_;
            case IX:    return state.IX;
            case IY:    return state.IY;
            case SP:    return state.SP;
            case PC:    return state.PC;
            default:
                YAKC_ASSERT(false);
                return 0;
        }
    }
    /// get a string-name for a register
    static const char* reg_name(reg r) {
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
    /// perform a reset (RESET pin triggered)
    void reset() {
        state.PC = 0;
        state.IM = 0;
        state.IFF1 = false;
        state.IFF2 = false;
        state.I = 0;
        state.R = 0;
    }
    /// helper to test expected flag bitmask
    bool test_flags(ubyte expected) const {
        // mask out undocumented flags
        ubyte undoc = ~(XF|YF);
        return (state.F & undoc) == expected;
    }

    /// fetch an opcode byte and increment R register
    ubyte fetch_op() {
        state.R = (state.R + 1) & 0x7F;
        return mem.r8(state.PC++);
    }

    /// perform an add, return result, and update flags
    ubyte add8(ubyte acc, ubyte add) {
        int r = int(acc) + int(add);
        ubyte f = (r & 0xFF) ? ((r & 0x80) ? SF : 0) : ZF;
        if (r > 0xFF) f |= CF;
        if ((r & 0xF) < (acc & 0xF)) f |= HF;
        if (((acc&0x80) == (add&0x80)) && ((r&0x80) != (acc&0x80))) f |= VF;
        state.F = f;
        return (ubyte)r;
    }
    /// perform an adc, return result and update flags
    ubyte adc8(ubyte acc, ubyte add) {
        if (state.F & CF) {
            int r = int(acc) + int(add) + 1;
            ubyte f = r ? ((r & 0x80) ? SF : 0) : ZF;
            if (r > 0xFF) f |= CF;
            if ((r & 0xF) <= (acc & 0xF)) f |= HF;
            if (((acc&0x80) == (add&0x80)) && ((r&0x80) != (acc&0x80))) f |= VF;
            state.F = f;
            return (ubyte)r;
        }
        else {
            return add8(acc, add);
        }
    }
    /// perform a sub, return result, and update flags
    ubyte sub8(ubyte acc, ubyte sub) {
        int r = int(acc) - int(sub);
        ubyte f = NF | (r ? ((r & 0x80) ? SF : 0) : ZF);
        if (r < 0) f |= CF;
        if ((r & 0xF) > (acc & 0xF)) f |= HF;
        if (((acc&0x80) != (sub&0x80)) && ((r&0x80) != (acc&0x80))) f |= VF;
        state.F = f;
        return (ubyte)r;
    }
    /// perform an sbc, return result and update flags
    ubyte sbc8(ubyte acc, ubyte sub) {
        if (state.F & CF) {
            int r = int(acc) - int(sub) - 1;
            ubyte f = NF | (r ? ((r & 0x80) ? SF : 0) : ZF);
            if (r < 0) f |= CF;
            if ((r & 0xF) >= (acc & 0xF)) f |= HF;
            if (((acc&0x80) != (sub&0x80)) && ((r&0x80) != (acc&0x80))) f |= VF;
            state.F = f;
            return (ubyte)r;
        }
        else {
            return sub8(acc, sub);
        }
    }
    /// perform an 8-bit inc and set flags
    ubyte inc8(ubyte val) {
        ubyte r = val + 1;
        ubyte f = (r ? ((r & 0x80) ? SF : 0) : ZF);
        if ((r & 0xF) == 0) f |= HF;
        if (r == 0x80) f |= VF;
        state.F = f | (state.F & CF);
        return r;
    }
    /// perform an 8-bit dec and set flags
    ubyte dec8(ubyte val) {
        ubyte r = val - 1;
        ubyte f = NF | (r ? ((r & 0x80) ? SF : 0) : ZF);
        if ((r & 0xF) == 0xF) f |= HF;
        if (r == 0x7F) f |= VF;
        state.F = f | (state.F & CF);
        return r;
    }
    /// implement the LDI instruction
    void ldi() {
        ubyte val = mem.r8(state.HL);
        mem.w8(state.DE, val);
        val += state.A;
        ubyte f = state.F & (SF|ZF|CF);
        if (val & 0x02) f |= YF;
        if (val & 0x08) f |= XF;
        state.HL++;
        state.DE++;
        state.BC--;
        if (state.BC) {
            f |= VF;
        }
        state.F = f;
    }
    /// implement the LDIR instruction, return number of T-states
    int ldir() {
        ldi();
        if (state.BC != 0) {
            state.PC -= 2;
            return 21;
        }
        else {
            return 16;
        }
    }
    /// implement the LDD instruction
    void ldd() {
        ubyte val = mem.r8(state.HL);
        mem.w8(state.DE, val);
        val += state.A;
        ubyte f = state.F & (SF|ZF|CF);
        if (val & 0x02) f |= YF;
        if (val & 0x08) f |= XF;
        state.HL--;
        state.DE--;
        state.BC--;
        if (state.BC) {
            f |= VF;
        }
        state.F = f;
    }
    /// implement the LDDR instruction, return number of T-states
    int lddr() {
        ldd();
        if (state.BC != 0) {
            state.PC -= 2;
            return 21;
        }
        else {
            return 16;
        }
    }

    /// get the SF|ZF|PF flags for a value
    static ubyte szp(ubyte val) {
        int p = 0;
        if (val & (1<<0)) p++;
        if (val & (1<<1)) p++;
        if (val & (1<<2)) p++;
        if (val & (1<<3)) p++;
        if (val & (1<<4)) p++;
        if (val & (1<<5)) p++;
        if (val & (1<<6)) p++;
        if (val & (1<<7)) p++;
        ubyte f = val ? val & SF : ZF;
        f |= (val & (YF|XF));   // undocumented flag bits 3 and 5
        f |= p & 1 ? 0 : PF;
        return f;
    }
    /// get flags for the LD A,I and LD A,R instructions
    static ubyte sziff2(ubyte val, bool iff2) {
        ubyte f = val ? val & SF : ZF;
        f |= (val & (YF|XF));   // undocumented flag bits 3 and 5
        if (iff2) f |= PF;
        return f;
    }
    /// rotate left, copy sign bit into CF,
    ubyte rlc8(ubyte val, bool flags_szp) {
        ubyte r = val<<1|val>>7;
        ubyte f = (val & 0x80) ? CF : 0;
        if (flags_szp) {
            state.F = f | szp(r);
        }
        else {
            state.F = f | (state.F & (SF|ZF|PF));
        }
        return r;
    }
    /// rotate right, copy bit 0 into CF
    ubyte rrc8(ubyte val, bool flags_szp) {
        ubyte r = val>>1|val<<7;
        ubyte f = (val & 0x01) ? CF : 0;
        if (flags_szp) {
            state.F = f | szp(r);
        }
        else {
            state.F = f | (state.F & (SF|ZF|PF));
        }
        return r;
    }
    /// rotate left through carry bit
    ubyte rl8(ubyte val, bool flags_szp) {
        ubyte r = val<<1 | ((state.F & CF) ? 0x01:0x00);
        ubyte f = val & 0x80 ? CF : 0;
        if (flags_szp) {
            state.F = f | szp(r);
        }
        else {
            state.F = f | (state.F & (SF|ZF|PF));
        }
        return r;
    }
    /// rotate right through carry bit
    ubyte rr8(ubyte val, bool flags_szp) {
        ubyte r = val>>1 | ((state.F & CF) ? 0x80:0x00);
        ubyte f = val & 0x01 ? CF : 0;
        if (flags_szp) {
            state.F = f | szp(r);
        }
        else {
            state.F = f | (state.F & (SF|ZF|PF));
        }
        return r;
    }
    /// shift left into carry bit and update flags
    ubyte sla8(ubyte val) {
        ubyte r = val<<1;
        ubyte f = val & 0x80 ? CF : 0;
        state.F = f | szp(r);
        return r;
    }
    /// shift right into carry bit, preserve sign, update flags
    ubyte sra8(ubyte val) {
        ubyte r = (val>>1) | (val & 0x80);
        ubyte f = val & 0x01 ? CF : 0;
        state.F = f | szp(r);
        return r;
    }
    /// shift right into carry bit, update flags
    ubyte srl8(ubyte val) {
        ubyte r = val>>1;
        ubyte f = val & 0x01 ? CF : 0;
        state.F = f | szp(r);
        return r;
    }
    /// implements the RLD instruction
    void rld() {
        ubyte x = mem.r8(state.HL);
        ubyte tmp = state.A & 0xF;              // store A low nibble
        state.A = (state.A & 0xF0) | (x>>4);    // move (HL) high nibble into A low nibble
        x = (x<<4) | tmp;   // move (HL) low to high nibble, move A low nibble to (HL) low nibble
        mem.w8(state.HL, x);
        state.F = szp(state.A) | (state.F & CF);
    }
    /// implements the RRD instruction
    void rrd() {
        ubyte x = mem.r8(state.HL);
        ubyte tmp = state.A & 0xF;                  // store A low nibble
        state.A = (state.A & 0xF0) | (x & 0x0F);    // move (HL) low nibble to A low nibble
        x = (x >> 4) | (tmp << 4);  // move A low nibble to (HL) high nibble, and (HL) high nibble to (HL) low nibble
        mem.w8(state.HL, x);
        state.F = szp(state.A) | (state.F & CF);
    }
    /// implements the BIT test instruction, updates flags
    void bit(ubyte val, ubyte mask) {
        ubyte r = val & mask;
        ubyte f = r ? (r & SF) : (ZF|PF);
        f |= (r & (YF|XF));
        state.F = f | (state.F & CF);
    }

    /// special handling for the FD/DD CB bit opcodes
    void dd_fd_cb(ubyte lead) {
        int d = mem.rs8(state.PC++);
        uword addr;
        if (lead == 0xDD) {
            addr = state.IX + d;
        }
        else {
            addr = state.IY + d;
        }
        ubyte op = mem.r8(state.PC++);
        switch (op) {
            // RLC ([IX|IY]+d)
            case 0x06:
                mem.w8(addr, rlc8(mem.r8(addr), true));
                state.T += 23;
                break;
            // RRC ([IX|IY]+d)
            case 0x0E:
                mem.w8(addr, rrc8(mem.r8(addr), true));
                state.T += 23;
                break;
            // RL ([IX|IY]+d)
            case 0x16:
                mem.w8(addr, rl8(mem.r8(addr), true));
                state.T += 23;
                break;
            // RR ([IX|IY]+d)
            case 0x1E:
                mem.w8(addr, rr8(mem.r8(addr), true));
                state.T += 23;
                break;
            // SLA ([IX|IY]+d)
            case 0x26:
                mem.w8(addr, sla8(mem.r8(addr)));
                state.T += 23;
                break;
            // SRA ([IX|IY]+d)
            case 0x2E:
                mem.w8(addr, sra8(mem.r8(addr)));
                state.T += 23;
                break;
            // SRL ([IX|IY]+d)
            case 0x3E:
                mem.w8(addr, srl8(mem.r8(addr)));
                state.T += 23;
                break;
            // BIT b,([IX|IY]+d)
            case 0x46 | (0<<3):
            case 0x46 | (1<<3):
            case 0x46 | (2<<3):
            case 0x46 | (3<<3):
            case 0x46 | (4<<3):
            case 0x46 | (5<<3):
            case 0x46 | (6<<3):
            case 0x46 | (7<<3):
                bit(mem.r8(addr), 1<<((op>>3)&7));
                state.T += 20;
                break;
            // RES b,([IX|IY]+d)
            case 0x86 | (0<<3):
            case 0x86 | (1<<3):
            case 0x86 | (2<<3):
            case 0x86 | (3<<3):
            case 0x86 | (4<<3):
            case 0x86 | (5<<3):
            case 0x86 | (6<<3):
            case 0x86 | (7<<3):
                mem.w8(addr, mem.r8(addr) & ~(1<<((op>>3)&7)));
                state.T += 23;
                break;
            // SET b,([IX|IY]+d)
            case 0xC6 | (0<<3):
            case 0xC6 | (1<<3):
            case 0xC6 | (2<<3):
            case 0xC6 | (3<<3):
            case 0xC6 | (4<<3):
            case 0xC6 | (5<<3):
            case 0xC6 | (6<<3):
            case 0xC6 | (7<<3):
                mem.w8(addr, mem.r8(addr) | (1<<((op>>3)&7)));
                state.T += 23;
                break;

            // unknown opcode
            default:
                YAKC_ASSERT(false);
                break;
        }
    }

    /// execute a single instruction and update machine state
    void step();
};

} // namespace
#include "opcodes.h"
