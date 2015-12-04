#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z80
    @brief the Z80 (or rather U880) cpu
*/
#include "core/common.h"
#include "core/memory.h"
#include "core/z80int.h"

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
        union {
            struct { ubyte IXL, IXH; };
            uword IX;
        };
        union {
            struct { ubyte IYL, IYH; };
            uword IY;
        };
        uword SP;
        uword PC;

        /// CPU is in HALT state
        bool HALT;
        /// the interrupt-enable flip-flops
        bool IFF1;
        bool IFF2;
        /// invalid or unknown instruction has been encountered
        bool INV;
        /// the interrupt mode (0, 1 or 2)
        ubyte IM;
    } state;

    /// flag lookup table for SZP flag combinations
    ubyte szp[256];
    /// inc8 flags
    ubyte inc8_flags[256];
    /// dec8 flags
    ubyte dec8_flags[256];
    /// flag lookup table for add8
    ubyte add8_flags[256][256];
    /// flag lookup table for sub8
    ubyte sub8_flags[256][256];

    /// memory map
    memory mem;

    /// input hook typedef, take a port number, return value at that port
    typedef ubyte (*cb_in)(void* userdata, uword port);
    /// output hook typedef, write 8-bit value to output port
    typedef void (*cb_out)(void* userdata, uword port, ubyte val);

    /// user-provided in-handler
    cb_in in_func;
    /// user-provided out-handler
    cb_out out_func;
    /// input/output userdata
    void* inout_userdata;

    /// highest priority interrupt controller in daisy chain
    z80int* irq_device;
    /// an interrupt request has been received
    bool irq_received;
    /// delayed-interrupt-enable flag set bei ei()
    bool enable_interrupt;
    /// break on invalid opcode?
    bool break_on_invalid_opcode;

    /// constructor
    z80();

    /// one-time init
    void init(cb_in func_in, cb_out func_out, void* userdata);
    /// initialize the flag lookup tables
    void init_flag_tables();
    /// connect the highest priority interrupt controller device
    void connect_irq_device(z80int* device);
    /// perform a reset (RESET pin triggered)
    void reset();
    /// execute a single instruction, return number of cycles
    unsigned int step();

    /// called when invalid opcode has been hit
    unsigned int invalid_opcode(uword opsize);

    /// receive an interrupt request
    static void irq(void* self);
    /// handle an interrupt request, must be called after step(), return num tstates taken
    int handle_irq();
    /// implement the RETI instruction
    void reti();
    /// implement the EI instruction
    void ei();
    /// implement the DI instruction
    void di();

    /// helper method to swap 2 16-bit registers
    static void swap16(uword& r0, uword& r1);
    /// helper to test expected flag bitmask (FIXME: move to z80dbg?)
    bool test_flags(ubyte expected) const;
    /// fetch an opcode byte and increment R register
    ubyte fetch_op();
    /// call in-handler, return result
    ubyte in(uword port);
    /// call out-handler
    void out(uword port, ubyte val);

    /// get flags for the LD A,I and LD A,R instructions
    static ubyte sziff2(ubyte val, bool iff2);

    /// halt instruction
    void halt();
    /// perform an 8-bit add, return result, and update flags
    ubyte add8(ubyte acc, ubyte add);
    /// perform an 8-bit adc, return result and update flags
    ubyte adc8(ubyte acc, ubyte add);
    /// perform an 8-bit sub, return result, and update flags
    ubyte sub8(ubyte acc, ubyte sub);
    /// perform an 8-bit sbc, return result and update flags
    ubyte sbc8(ubyte acc, ubyte sub);
    /// perform an 8-bit inc and set flags
    ubyte inc8(ubyte val);
    /// perform an 8-bit dec and set flags
    ubyte dec8(ubyte val);
    /// perform an 16-bit add, update flags and return result
    uword add16(uword acc, uword val);
    /// perform an 16-bit adc, update flags and return result
    uword adc16(uword acc, uword val);
    /// perform an 16-bit sbc, update flags and return result
    uword sbc16(uword acc, uword val);
    /// implement the LDI instruction
    void ldi();
    /// implement the LDIR instruction, return number of T-states
    int ldir();
    /// implement the LDD instruction
    void ldd();
    /// implement the LDDR instruction, return number of T-states
    int lddr();
    /// implement the CPI instruction
    void cpi();
    /// implement the CPIR instruction, return number of T-states
    int cpir();
    /// implement the CPD instruction
    void cpd();
    /// implement the CPDR instruction, return number of T-states
    int cpdr();
    /// return flags for ini/ind instruction
    ubyte ini_ind_flags(ubyte io_val, int c_add);
    /// implement the INI instruction
    void ini();
    /// implement the INIR instruction, return number of T-states
    int inir();
    /// implement the IND instruction
    void ind();
    /// implement the INDR instruction, return number of T-states
    int indr();
    /// return flags for outi/outd instruction
    ubyte outi_outd_flags(ubyte io_val);
    /// implement the OUTI instruction
    void outi();
    /// implement the OTIR instructor, return number of T-states
    int otir();
    /// implment the OUTD instruction
    void outd();
    /// implement the OTDR instruction, return number of T-states
    int otdr();
    /// implement the DAA instruction
    void daa();
    /// rotate left, copy sign bit into CF,
    ubyte rlc8(ubyte val, bool flags_szp);
    /// rotate right, copy bit 0 into CF
    ubyte rrc8(ubyte val, bool flags_szp);
    /// rotate left through carry bit
    ubyte rl8(ubyte val, bool flags_szp);
    /// rotate right through carry bit
    ubyte rr8(ubyte val, bool flags_szp);
    /// shift left into carry bit and update flags
    ubyte sla8(ubyte val);
    /// undocumented: shift left into carry bit, update flags
    ubyte sll8(ubyte val);
    /// shift right into carry bit, preserve sign, update flags
    ubyte sra8(ubyte val);
    /// shift right into carry bit, update flags
    ubyte srl8(ubyte val);
    /// implements the RLD instruction
    void rld();
    /// implements the RRD instruction
    void rrd();
    /// implements the BIT test instruction, updates flags
    void bit(ubyte val, ubyte mask);
    /// undocumented register-autocopy for the DD/FD CB instructions
    void undoc_autocopy(ubyte reg, ubyte val);
    /// special handling for the FD/DD CB bit opcodes
    int dd_fd_cb(ubyte lead);
};

//------------------------------------------------------------------------------
inline z80::z80() :
in_func(nullptr),
out_func(nullptr),
inout_userdata(nullptr),
irq_device(nullptr),
irq_received(false),
enable_interrupt(false),
break_on_invalid_opcode(false) {
    YAKC_MEMSET(&this->state, 0, sizeof(this->state));
}

//------------------------------------------------------------------------------
inline void
z80::init(cb_in func_in, cb_out func_out, void* userdata) {
    YAKC_ASSERT(func_in && func_out);
    this->init_flag_tables();
    this->reset();
    this->in_func = func_in;
    this->out_func = func_out;
    this->inout_userdata = userdata;
}

//------------------------------------------------------------------------------
#define YAKC_SZ(val) ((val&0xFF)?(val&SF):ZF)
inline void
z80::init_flag_tables() {
    for (int val = 0; val < 256; val++) {
        int p = 0;
        if (val & (1<<0)) p++;
        if (val & (1<<1)) p++;
        if (val & (1<<2)) p++;
        if (val & (1<<3)) p++;
        if (val & (1<<4)) p++;
        if (val & (1<<5)) p++;
        if (val & (1<<6)) p++;
        if (val & (1<<7)) p++;
        ubyte f = val?(val&SF):ZF;
        f |= (val & (YF|XF));   // undocumented flag bits 3 and 5
        f |= p & 1 ? 0 : PF;
        this->szp[val] = f;
    }
    for (int val = 0; val < 256; val++) {
        ubyte f = YAKC_SZ(val);
        if ((val & 0xF) == 0) f |= HF;
        if (val == 0x80) f |= VF;
        this->inc8_flags[val] = f;
    }
    for (int val = 0; val < 256; val++) {
        ubyte f = NF | YAKC_SZ(val);
        if ((val & 0xF) == 0xF) f |= HF;
        if (val == 0x7F) f |= VF;
        this->dec8_flags[val] = f;
    }
    for (int acc = 0; acc < 256; acc++) {
        for (int add = 0; add < 256; add++) {
            int r = acc + add;
            ubyte f = YAKC_SZ(r);
            if (r > 0xFF) f |= CF;
            if ((r & 0xF) < (acc & 0xF)) f |= HF;
            if (((acc&0x80) == (add&0x80)) && ((r&0x80) != (acc&0x80))) f |= VF;
            this->add8_flags[acc][add] = f;
        }
    }
    for (int acc = 0; acc < 256; acc++) {
        for (int sub = 0; sub < 256; sub++) {
            int r = int(acc) - int(sub);
            ubyte f = NF | YAKC_SZ(r);
            if (r < 0) f |= CF;
            if ((r & 0xF) > (acc & 0xF)) f |= HF;
            if (((acc&0x80) != (sub&0x80)) && ((r&0x80) != (acc&0x80))) f |= VF;
            this->sub8_flags[acc][sub] = f;
        }
    }
}

//------------------------------------------------------------------------------
inline void
z80::connect_irq_device(z80int* device) {
    YAKC_ASSERT(device);
    this->irq_device = device;
}

//------------------------------------------------------------------------------
inline void
z80::reset() {
    this->state.PC = 0;
    this->state.IM = 0;
    this->state.HALT = false;
    this->state.IFF1 = false;
    this->state.IFF2 = false;
    this->state.I = 0;
    this->state.R = 0;
    this->irq_received = false;
    this->enable_interrupt = false;
}

//------------------------------------------------------------------------------
inline void
z80::halt() {
    this->state.HALT = true;
    this->state.PC--;
}

//------------------------------------------------------------------------------
inline void
z80::irq(void* userdata) {
    z80* self = (z80*) userdata;
    self->irq_received = true;
}

//------------------------------------------------------------------------------
inline int
z80::handle_irq() {
    int tstates = 0;
    if (this->irq_received) {
        tstates += 2;
        // we don't implement MODE0 or MODE1 (yet?)
        YAKC_ASSERT(2 == this->state.IM);

        // leave halt
        if (this->state.HALT) {
            this->state.HALT = false;
            this->state.PC++;
        }

        this->irq_received = false;
        if (this->irq_device) {
            if (this->state.IFF1) {
                // handle interrupt
                this->state.IFF1 = this->state.IFF2 = false;
                ubyte vec = this->irq_device->interrupt_acknowledged();
                uword addr = (this->state.I<<8)|(vec&0xFE);
                this->state.SP -= 2;
                this->mem.w16(this->state.SP, this->state.PC);
                this->state.PC = this->mem.r16(addr);
                tstates += 19;
            }
            else {
                // interrupts are disabled, notify daisy chain
                this->irq_device->interrupt_cancelled();
            }
        }
    }
    return tstates;
}

//------------------------------------------------------------------------------
inline void
z80::reti() {
    // this is the same as RET
    this->state.PC = mem.r16(state.SP);
    this->state.SP += 2;
    // ...notify daisy chain, if configured
    if (this->irq_device) {
        this->irq_device->reti();
    }
}

//------------------------------------------------------------------------------
inline void
z80::ei() {
    // NOTE: interrupts are actually enabled *after the next instruction*,
    // thus we only set a flag here, the IFF flags will then be changed
    // during the next instruction decode in step()
    this->enable_interrupt = true;
}

//------------------------------------------------------------------------------
inline void
z80::di() {
    this->state.IFF1 = false;
    this->state.IFF2 = false;
}

//------------------------------------------------------------------------------
inline unsigned int
z80::invalid_opcode(uword opsize) {
    if (this->break_on_invalid_opcode) {
        state.INV = true;
        state.PC -= opsize;     // stuck on invalid opcode
    }
    return 4;
}

//------------------------------------------------------------------------------
inline void
z80::swap16(uword& r0, uword& r1) {
    uword tmp = r0;
    r0 = r1;
    r1 = tmp;
}

//------------------------------------------------------------------------------
inline bool
z80::test_flags(ubyte expected) const {
    // mask out undocumented flags
    ubyte undoc = ~(XF|YF);
    return (state.F & undoc) == expected;
}

//------------------------------------------------------------------------------
inline ubyte
z80::fetch_op() {
    state.R = (state.R + 1) & 0x7F;
    return mem.r8(state.PC++);
}

//------------------------------------------------------------------------------
inline ubyte
z80::in(uword port) {
    if (in_func) {
        return in_func(inout_userdata, port);
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
inline void
z80::out(uword port, ubyte val) {
    if (out_func) {
        return out_func(inout_userdata, port, val);
    }
}

//------------------------------------------------------------------------------
inline ubyte
z80::sziff2(ubyte val, bool iff2) {
    ubyte f = YAKC_SZ(val);
    f |= (val & (YF|XF));   // undocumented flag bits 3 and 5
    if (iff2) f |= PF;
    return f;
}

//------------------------------------------------------------------------------
inline ubyte
z80::add8(ubyte acc, ubyte add) {
    state.F = add8_flags[acc][add];
    return acc + add;
}

//------------------------------------------------------------------------------
inline ubyte
z80::adc8(ubyte acc, ubyte add) {
    if (state.F & CF) {
        // don't waste flag table space for rarely used instructions
        int r = int(acc) + int(add) + 1;
        ubyte f = YAKC_SZ(r);
        if (r > 0xFF) f |= CF;
        if ((r & 0xF) <= (acc & 0xF)) f |= HF;
        if (((acc&0x80) == (add&0x80)) && ((r&0x80) != (acc&0x80))) f |= VF;
        state.F = f;
        return ubyte(r);
    }
    else {
        return add8(acc, add);
    }
}

//------------------------------------------------------------------------------
inline ubyte
z80::sub8(ubyte acc, ubyte sub) {
    state.F = sub8_flags[acc][sub];
    return acc - sub;
}

//------------------------------------------------------------------------------
inline ubyte
z80::sbc8(ubyte acc, ubyte sub) {
    if (state.F & CF) {
        // don't waste flag table space for rarely used instructions
        int r = int(acc) - int(sub) - 1;
        ubyte f = NF | YAKC_SZ(r);
        if (r < 0) f |= CF;
        if ((r & 0xF) >= (acc & 0xF)) f |= HF;
        if (((acc&0x80) != (sub&0x80)) && ((r&0x80) != (acc&0x80))) f |= VF;
        state.F = f;
        return ubyte(r);
    }
    else {
        return sub8(acc, sub);
    }
}

//------------------------------------------------------------------------------
inline ubyte
z80::inc8(ubyte val) {
    ubyte r = val + 1;
    state.F = this->inc8_flags[r] | (state.F & CF);
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::dec8(ubyte val) {
    ubyte r = val - 1;
    state.F = this->dec8_flags[r] | (state.F & CF);
    return r;
}

//------------------------------------------------------------------------------
inline uword
z80::add16(uword acc, uword val) {
    unsigned int res = acc + val;
    // flag computation taken from MAME
    state.F = (state.F & (SF|ZF|VF)) |
              (((acc^res^val)>>8)&HF)|
              ((res>>16) & CF) | ((res >> 8) & (YF|XF));
    return (uword)res;
}

//------------------------------------------------------------------------------
inline uword
z80::adc16(uword acc, uword val) {
    unsigned int res = acc + val + (state.F & CF);
    // flag computation taken from MAME
    state.F = (((acc^res^val)>>8)&HF) |
              ((res>>16)&CF) |
              ((res>>8)&(SF|YF|XF)) |
              ((res & 0xFFFF) ? 0 : ZF) |
              (((val^acc^0x8000) & (val^res)&0x8000)>>13);
    return res;
}

//------------------------------------------------------------------------------
inline uword
z80::sbc16(uword acc, uword val) {
    unsigned int res = acc - val - (state.F & CF);
    // flag computation taken from MAME
    state.F = (((acc^res^val)>>8)&HF) | NF |
              ((res>>16)&CF) |
              ((res>>8) & (SF|YF|XF)) |
              ((res & 0xFFFF) ? 0 : ZF) |
              (((val^acc) & (acc^res)&0x8000)>>13);
    return res;
}

//------------------------------------------------------------------------------
inline void
z80::ldi() {
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

//------------------------------------------------------------------------------
inline int
z80::ldir() {
    ldi();
    if (state.BC != 0) {
        state.PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80::ldd() {
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

//------------------------------------------------------------------------------
inline int
z80::lddr() {
    ldd();
    if (state.BC != 0) {
        state.PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80::cpi() {
    int r = int(state.A) - int(mem.r8(state.HL));
    ubyte f = NF | (state.F & CF) | YAKC_SZ(r);
    if ((r & 0xF) > (state.A & 0xF)) {
        f |= HF;
        r--;
    }
    if (r & 0x02) f |= YF;
    if (r & 0x08) f |= XF;
    state.HL++;
    state.BC--;
    if (state.BC) {
        f |= VF;
    }
    state.F = f;
}

//------------------------------------------------------------------------------
inline int
z80::cpir() {
    cpi();
    if ((state.BC != 0) && !(state.F & ZF)) {
        state.PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80::cpd() {
    int r = int(state.A) - int(mem.r8(state.HL));
    ubyte f = NF | (state.F & CF) | YAKC_SZ(r);
    if ((r & 0xF) > (state.A & 0xF)) {
        f |= HF;
        r--;
    }
    if (r & 0x02) f |= YF;
    if (r & 0x08) f |= XF;
    state.HL--;
    state.BC--;
    if (state.BC) {
        f |= VF;
    }
    state.F = f;
}

//------------------------------------------------------------------------------
inline int
z80::cpdr() {
    cpd();
    if ((state.BC != 0) && !(state.F & ZF)) {
        state.PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline ubyte
z80::ini_ind_flags(ubyte io_val, int c_add) {
    // NOTE: most INI flag settings are undocumented in the official
    // docs, so this is taken from MAME, there's also more
    // information here: http://www.z80.info/z80undoc3.txt
    ubyte f = state.B ? state.B & SF : ZF;
    if (io_val & SF) f |= NF;
    unsigned int t = (unsigned int)((state.C+c_add)&0xFF) + (unsigned int)io_val;
    if (t & 0x100) f |= HF|CF;
    f |= szp[ubyte(t & 0x07)^state.B] & PF;
    return f;
}

//------------------------------------------------------------------------------
inline void
z80::ini() {
    ubyte io_val = in(state.BC);
    state.B--;
    mem.w8(state.HL++, io_val);
    state.F = ini_ind_flags(io_val, +1);
}

//------------------------------------------------------------------------------
inline int
z80::inir() {
    ini();
    if (state.B != 0) {
        state.PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80::ind() {
    ubyte io_val = in(state.BC);
    state.B--;
    mem.w8(state.HL--, io_val);
    state.F = ini_ind_flags(io_val, -1);
}

//------------------------------------------------------------------------------
inline int
z80::indr() {
    ind();
    if (state.B != 0) {
        state.PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline ubyte
z80::outi_outd_flags(ubyte io_val) {
    // NOTE: most OUTI flag settings are undocumented in the official
    // docs, so this is taken from MAME, there's also more
    // information here: http://www.z80.info/z80undoc3.txt
    ubyte f = state.B ? state.B & SF : ZF;
    if (io_val & SF) f |= NF;
    unsigned int t = (unsigned int)state.L + (unsigned int)io_val;
    if (t & 0x100) f |= HF|CF;
    f |= szp[ubyte(t & 0x07)^state.B] & PF;
    return f;
}

//------------------------------------------------------------------------------
inline void
z80::outi() {
    ubyte io_val = mem.r8(state.HL++);
    state.B--;
    out(state.BC, io_val);
    state.F = outi_outd_flags(io_val);
}

//------------------------------------------------------------------------------
inline int
z80::otir() {
    outi();
    if (state.B != 0) {
        state.PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80::outd() {
    ubyte io_val = mem.r8(state.HL--);
    state.B--;
    out(state.BC, io_val);
    state.F = outi_outd_flags(io_val);
}

//------------------------------------------------------------------------------
inline int
z80::otdr() {
    outd();
    if (state.B != 0) {
        state.PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80::daa() {
    // from MAME and http://www.z80.info/zip/z80-documented.pdf
    ubyte val = state.A;
    if (state.F & NF) {
        if (((state.A & 0xF) > 0x9) || (state.F & HF)) {
            val -= 0x06;
        }
        if ((state.A > 0x99) || (state.F & CF)) {
            val -= 0x60;
        }
    }
    else {
        if (((state.A & 0xF) > 0x9) || (state.F & HF)) {
            val += 0x06;
        }
        if ((state.A > 0x99) || (state.F & CF)) {
            val += 0x60;
        }
    }
    state.F &= CF|NF;
    state.F |= (state.A > 0x99) ? CF:0;
    state.F |= (state.A^val) & HF;
    state.F |= szp[val];
    state.A = val;
}

//------------------------------------------------------------------------------
inline ubyte
z80::rlc8(ubyte val, bool flags_szp) {
    ubyte r = val<<1|val>>7;
    ubyte f = (val & 0x80) ? CF : 0;
    if (flags_szp) {
        state.F = f | szp[r];
    }
    else {
        state.F = f | (state.F & (SF|ZF|PF));
    }
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::rrc8(ubyte val, bool flags_szp) {
    ubyte r = val>>1|val<<7;
    ubyte f = val & CF;
    if (flags_szp) {
        state.F = f | szp[r];
    }
    else {
        state.F = f | (state.F & (SF|ZF|PF));
    }
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::rl8(ubyte val, bool flags_szp) {
    ubyte r = val<<1 | ((state.F & CF) ? 0x01:0x00);
    ubyte f = val & 0x80 ? CF : 0;
    if (flags_szp) {
        state.F = f | szp[r];
    }
    else {
        state.F = f | (state.F & (SF|ZF|PF));
    }
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::rr8(ubyte val, bool flags_szp) {
    ubyte r = val>>1 | ((state.F & CF) ? 0x80:0x00);
    ubyte f = val & CF;
    if (flags_szp) {
        state.F = f | szp[r];
    }
    else {
        state.F = f | (state.F & (SF|ZF|PF));
    }
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::sla8(ubyte val) {
    ubyte r = val<<1;
    state.F = (val & 0x80 ? CF : 0) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::sll8(ubyte val) {
    // undocument! sll8 is identical with sla8 but inserts a 1 into the LSB
    ubyte r = (val<<1) | 1;
    state.F = (val & 0x80 ? CF : 0) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::sra8(ubyte val) {
    ubyte r = (val>>1) | (val & 0x80);
    state.F = (val & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::srl8(ubyte val) {
    ubyte r = val>>1;
    state.F = (val & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::rld() {
    ubyte x = mem.r8(state.HL);
    ubyte tmp = state.A & 0xF;              // store A low nibble
    state.A = (state.A & 0xF0) | (x>>4);    // move (HL) high nibble into A low nibble
    x = (x<<4) | tmp;   // move (HL) low to high nibble, move A low nibble to (HL) low nibble
    mem.w8(state.HL, x);
    state.F = szp[state.A] | (state.F & CF);
}

//------------------------------------------------------------------------------
inline void
z80::rrd() {
    ubyte x = mem.r8(state.HL);
    ubyte tmp = state.A & 0xF;                  // store A low nibble
    state.A = (state.A & 0xF0) | (x & 0x0F);    // move (HL) low nibble to A low nibble
    x = (x >> 4) | (tmp << 4);  // move A low nibble to (HL) high nibble, and (HL) high nibble to (HL) low nibble
    mem.w8(state.HL, x);
    state.F = szp[state.A] | (state.F & CF);
}

//------------------------------------------------------------------------------
inline void
z80::bit(ubyte val, ubyte mask) {
    ubyte r = val & mask;
    ubyte f = HF | (r ? (r & SF) : (ZF|PF));
    f |= (val & (YF|XF));
    state.F = f | (state.F & CF);
}

//------------------------------------------------------------------------------
inline void
z80::undoc_autocopy(ubyte reg, ubyte val) {
    // this is for the undocumented DD CB and FB CB instruction which autocopy
    // the result into an 8-bit register (except for the F register)
    switch (reg) {
        case 0: state.B = val; break;
        case 1: state.C = val; break;
        case 2: state.D = val; break;
        case 3: state.E = val; break;
        case 4: state.H = val; break;
        case 5: state.L = val; break;
        case 7: state.A = val; break;
    }
}

//------------------------------------------------------------------------------
inline int
z80::dd_fd_cb(ubyte lead) {
    int d = mem.rs8(state.PC++);
    uword addr;
    if (lead == 0xDD) {
        addr = state.IX + d;
    }
    else {
        addr = state.IY + d;
    }
    ubyte val;
    ubyte op = mem.r8(state.PC++);
    switch (op) {
        // RLC ([IX|IY]+d) -> r (except F)
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
            val = rlc8(mem.r8(addr), true);
            undoc_autocopy(op, val);
            mem.w8(addr, val);
            return 23;
        // RRC ([IX|IY]+d)
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0B:
        case 0x0C:
        case 0x0D:
        case 0x0E:
        case 0x0F:
            val = rrc8(mem.r8(addr), true);
            undoc_autocopy(op-0x08, val);
            mem.w8(addr, val);
            return 23;
        // RL ([IX|IY]+d)
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
            val = rl8(mem.r8(addr), true);
            undoc_autocopy(op-0x10, val);
            mem.w8(addr, val);
            return 23;
        // RR ([IX|IY]+d)
        case 0x18:
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
            val = rr8(mem.r8(addr), true);
            undoc_autocopy(op-0x18, val);
            mem.w8(addr, val);
            return 23;
        // SLA ([IX|IY]+d)
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
            val = sla8(mem.r8(addr));
            undoc_autocopy(op-0x20, val);
            mem.w8(addr, val);
            return 23;
        // SRA ([IX|IY]+d)
        case 0x28:
        case 0x29:
        case 0x2A:
        case 0x2B:
        case 0x2C:
        case 0x2D:
        case 0x2E:
        case 0x2F:
            val = sra8(mem.r8(addr));
            undoc_autocopy(op-0x28, val);
            mem.w8(addr, val);
            return 23;
        // SLL ([IX|IY]+d)
        case 0x30:
        case 0x31:
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x35:
        case 0x36:
        case 0x37:
            val = sll8(mem.r8(addr));
            undoc_autocopy(op-0x30, val);
            mem.w8(addr, val);
            return 23;
        // SRL ([IX|IY]+d)
        case 0x38:
        case 0x39:
        case 0x3A:
        case 0x3B:
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
            val = srl8(mem.r8(addr));
            undoc_autocopy(op-0x38, val);
            mem.w8(addr, val);
            return 23;
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
            return 20;
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
            return 23;
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
            return 23;

        // unknown opcode
        default:
            return invalid_opcode(4);
    }
}

} // namespace
#include "opcodes.h"
