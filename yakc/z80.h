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

#define READ8(addr) this->mem->r8(addr)
#define WRITE8(addr, b) this->mem->w8(addr,b)
#define READ16(addr) this->mem->r16(addr)
#define WRITE16(addr) this->mem->w16(addr)
#define F this->state.f
#define A this->state.a
#define AF this->state.af
#define C this->state.c
#define B this->state.b
#define BC this->state.bc
#define E this->state.e
#define D this->state.d
#define DE this->state.de
#define L this->state.l
#define H this->state.h
#define HL this->state.hl
#define PC this->state.pc
#define IM this->state.im
#define IFF1 this->state.iff1
#define IFF2 this->state.iff2
#define I this->state.i
#define R this->state.r
#define T_ADD(x) this->state.t+=x
#define SZHV_INC(x) this->szhv_inc(x)


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
            struct { ubyte f, a; };
            uword af;
        };
        union {
            struct { ubyte c, b; };
            uword bc;
        };
        union {
            struct { ubyte e, d; };
            uword de;
        };
        union {
            struct { ubyte l, h; };
            uword hl;
        };

        /// shadow register set
        uword af_;
        uword bc_;
        uword de_;
        uword hl_;

        /// special registers
        ubyte i;
        ubyte r;
        uword ix;
        uword iy;
        uword sp;
        uword pc;

        /// CPU is in HALT state
        bool halt;
        /// the interrupt-enable flip-flops
        bool iff1;
        bool iff2;
        /// the interrupt mode (0, 1 or 2)
        ubyte im;
        /// current T cycle counter
        unsigned int t;
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
    /// perform a reset (RESET pin triggered)
    void reset() {
        PC = 0;
        IM = 0;
        IFF1 = false;
        IFF2 = false;
        I = 0;
        R = 0;
    }
    /// update flags, add or adc w/o carry set (taken from MAME's z80)
    void szhvc_add(int oldval, int newval) {
        int val = newval - oldval;
        ubyte f = (newval) ? ((newval & 0x80) ? SF : 0) : ZF;
        f |= (newval & (YF | XF)); /* undocumented flag bits 5+3 */
        if ((newval & 0x0f) < (oldval & 0x0f)) f |= HF;
        if (newval < oldval) f |= CF;
        if ((val^oldval^0x80) & (val^newval) & 0x80) f |= VF;
        F = f;
    }

    /// update flags, adc with carry set (taken from MAME's z80)
    void szhvc_adc(int oldval, int newval) {
        int val = newval - oldval;
        ubyte f = (newval) ? ((newval & 0x80) ? SF : 0) : ZF;
        f |= (newval & (YF | XF)); /* undocumented flag bits 5+3 */
        if ((newval & 0x0f) <= (oldval & 0x0f)) f |= HF;
        if (newval <= oldval) f |= CF;
        if ((val^oldval^0x80) & (val^newval) & 0x80) f |= VF;
        F = f;
    }

    /// update flags, cp, sub or sbc w/o carry set (taken from MAME's z80)
    void szhvc_sub(int oldval, int newval) {
        int val = newval - oldval;
        ubyte f = NF | ((newval) ? ((newval & 0x80) ? SF : 0) : ZF);
        f |= (newval & (YF | XF)); /* undocumented flag bits 5+3 */
        if ((newval & 0x0f) > (oldval & 0x0f)) f |= HF;
        if (newval > oldval) f |= CF;
        if ((val^oldval) & (oldval^newval) & 0x80) f |= VF;
        F = f;
    }

    /// update flags, sbc with carry set
    void szhvc_sbc(int oldval, int newval) {
        int val = oldval - newval - 1;
        ubyte f = NF | ((newval) ? ((newval & 0x80) ? SF : 0) : ZF);
        f |= (newval & (YF | XF)); /* undocumented flag bits 5+3 */
        if ((newval & 0x0f) >= (oldval & 0x0f)) f |= HF;
        if (newval >= oldval) f |= CF;
        if ((val^oldval) & (oldval^newval) & 0x80) f |= VF;
        F = f;
    }

    /// update flags, increment 8-bit reg
    void szhv_inc(int val) {
        ubyte f = val ? val & SF : ZF;
        f |= (val & (YF | XF)); /* undocumented flag bits 5+3 */
        if (val == 0x80) f |= VF;
        if ((val & 0x0f) == 0x00) f |= HF;
        F = (F & CF) | f;
    }

    /// execute a single instruction and update machine state
    void exec() {
        // fetch first instruction byte
        ubyte i0 = READ8(PC++);
        switch (i0) {
            //# NOP
            case 0x00:
                T_ADD(4);
                break;
            //# LD BC,nn
            case 0x01:
                C = READ8(PC++);
                B = READ8(PC++);
                T_ADD(10);
                break;
            //# LD (BC),A
            case 0x02:
                WRITE8(BC, A);
                T_ADD(7);
                break;
            //# INC BC
            case 0x03:
                BC++;
                T_ADD(6);
                break;
            //# INC B
            case 0x04:
                B++;
                SZHV_INC(B);
                T_ADD(4);
                break;
            default:
                YAKC_ASSERT(false);
                break;
        }
    }
};

} // namespace yakc
