#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z80
    @brief the Z80 (or rather U880) cpu
*/
#include <string.h>
#include "yakc/common.h"
#include "yakc/memory.h"

namespace yakc {

class z80 {
public:

    /// flag bits
    enum {
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

    /// pointer to memory map
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
    /// execute a single instruction and update machine state
    void step();
};

} // namespace yakc
#include "opcodes.h"
