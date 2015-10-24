#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z80
    @brief the Z80 (or rather U880) cpu
*/
#include <cstring>
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
        YF = (1<<3),        // undocumented bit 3
        HF = (1<<4),        // half-carry
        XF = (1<<5),        // undocumented bit 5
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
    memory* mem;

    /// constructor
    z80() {
        std::memset(&this->state, 0, sizeof(this->state));
        this->mem = nullptr;
    }
    /// initialize the cpu
    void init(memory* mem_) {
        YAKC_ASSERT(mem_);
        std::memset(&this->state, 0, sizeof(this->state));
        this->mem = mem_;
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
    /// update flags, add or adc w/o carry set (taken from MAME's z80)
    void szhvc_add(int oldval, int newval) {
        int val = newval - oldval;
        ubyte f = (newval) ? ((newval & 0x80) ? SF : 0) : ZF;
        f |= (newval & (YF | XF)); /* undocumented flag bits 5+3 */
        if ((newval & 0x0f) < (oldval & 0x0f)) f |= HF;
        if (newval < oldval) f |= CF;
        if ((val^oldval^0x80) & (val^newval) & 0x80) f |= VF;
        state.F = f;
    }

    /// update flags, adc with carry set (taken from MAME's z80)
    void szhvc_adc(int oldval, int newval) {
        int val = newval - oldval;
        ubyte f = (newval) ? ((newval & 0x80) ? SF : 0) : ZF;
        f |= (newval & (YF | XF)); /* undocumented flag bits 5+3 */
        if ((newval & 0x0f) <= (oldval & 0x0f)) f |= HF;
        if (newval <= oldval) f |= CF;
        if ((val^oldval^0x80) & (val^newval) & 0x80) f |= VF;
        state.F = f;
    }

    /// update flags, cp, sub or sbc w/o carry set (taken from MAME's z80)
    void szhvc_sub(int oldval, int newval) {
        int val = newval - oldval;
        ubyte f = NF | ((newval) ? ((newval & 0x80) ? SF : 0) : ZF);
        f |= (newval & (YF | XF)); /* undocumented flag bits 5+3 */
        if ((newval & 0x0f) > (oldval & 0x0f)) f |= HF;
        if (newval > oldval) f |= CF;
        if ((val^oldval) & (oldval^newval) & 0x80) f |= VF;
        state.F = f;
    }

    /// update flags, sbc with carry set
    void szhvc_sbc(int oldval, int newval) {
        int val = oldval - newval - 1;
        ubyte f = NF | ((newval) ? ((newval & 0x80) ? SF : 0) : ZF);
        f |= (newval & (YF | XF)); /* undocumented flag bits 5+3 */
        if ((newval & 0x0f) >= (oldval & 0x0f)) f |= HF;
        if (newval >= oldval) f |= CF;
        if ((val^oldval) & (oldval^newval) & 0x80) f |= VF;
        state.F = f;
    }

    /// update flags, increment 8-bit reg
    void szhv_inc(int val) {
        ubyte f = val ? val & SF : ZF;
        f |= (val & (YF | XF)); /* undocumented flag bits 5+3 */
        if (val == 0x80) f |= VF;
        if ((val & 0x0f) == 0x00) f |= HF;
        state.F = (state.F & CF) | f;
    }

    /// execute a single instruction and update machine state
    void step();
};

} // namespace yakc
#include "opcodes.h"
