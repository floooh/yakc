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

    /// main register bank, accessible in various ways
    union {
        ubyte R8[16];       // B, C, D, E, H, L, F, A, IXH, IXL, IYH, IYL, W, Z, SPH, SPL,
        uword R16[8];       // BC, DE, HL, AF, IX, IY, WZ, SP
        struct {
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
            union {
                struct { ubyte F, A; };
                uword AF;
            };
            union {
                struct { ubyte IXL, IXH; };
                uword IX;
            };
            union {
                struct { ubyte IYL, IYH; };
                uword IY;
            };
            union {
                struct { ubyte Z, W; };
                uword WZ;
            };
            uword SP;
        };
    };

    /// shadow register set
    uword BC_;
    uword DE_;
    uword HL_;
    uword AF_;
    uword WZ_;
    /// instruction pointer
    uword PC;
    /// interrupt vector register
    ubyte I;
    /// memory refresh counter
    ubyte R;
    /// interrupt mode
    ubyte IM;

    /// CPU is in HALT state
    bool HALT;
    /// interrupt enable flipflops
    bool IFF1, IFF2;
    /// invalid instruction hit
    bool INV;

    #if YAKC_Z80_DECODER
    /// 8-bit register index map into R8
    int r[8];
    /// same as r[], but HL will never be patched to IX/IY
    int r2[8];
    /// 16-bit register index map with SP into R16
    int rp[4];
    /// 16-bit register index map with AF into R16
    int rp2[4];
    /// access to HL/IX/IY 16-bit register slot
    #define HLIXIY R16[rp[2]]
    #endif

    /// flag lookup table for SZP flag combinations
    ubyte szp[256];

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
    /// initialize the lookup tables
    void init_tables();
    /// connect the highest priority interrupt controller device
    void connect_irq_device(z80int* device);
    /// perform a reset (RESET pin triggered)
    void reset();
    /// helper to test expected flag bitmask (FIXME: move to z80dbg?)
    bool test_flags(ubyte expected) const;
    /// called when invalid opcode has been hit
    uint32_t invalid_opcode(uword opsize);
    /// helper method to swap 2 16-bit registers
    static void swap16(uword& r0, uword& r1);

    #if YAKC_Z80_DECODER
    /// check flag status for RET cc, JP cc, CALL cc, etc...
    bool cc(ubyte y) const;
    /// get address for (HL), (IX+d), (IY+d)
    uword iHLIXIYd(bool ext);
    /// perform a general ALU op (add, adc, sub, sbc, cp, and, or, xor)
    void alu8(ubyte alu, ubyte val);    
    #endif

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

    /// call in-handler, return result
    ubyte in(uword port);
    /// call out-handler
    void out(uword port, ubyte val);

    /// get flags for the LD A,I and LD A,R instructions
    static ubyte sziff2(ubyte val, bool iff2);

    /// halt instruction
    void halt();
    /// rst instruction (vec is 0x00, 0x08, 0x10, ...)
    void rst(ubyte vec);
    /// perform an 8-bit add and update flags
    void add8(ubyte add);
    /// perform an 8-bit adc and update flags
    void adc8(ubyte add);
    /// perform an 8-bit sub and update flags
    void sub8(ubyte sub);
    /// perform an 8-bit sbc and update flags
    void sbc8(ubyte sub);
    /// perform 8-bit compare (identical with sub8 but throw result away)
    void cp8(ubyte sub);
    /// perform a neg and update flags
    void neg8();
    /// perform an 8-bit and and update flags
    void and8(ubyte val);
    /// perform an 8-bit or and update flags
    void or8(ubyte val);
    /// perform an 8-bit xor and update flags
    void xor8(ubyte val);
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
    /// rotate left, copy sign bit into CF
    ubyte rlc8(ubyte val);
    /// rotate A left, copy sign bit into CF
    void rlca8();
    /// rotate right, copy bit 0 into CF
    ubyte rrc8(ubyte val);
    /// rotate A right, copy bit 0 into CF
    void rrca8();
    /// rotate left through carry bit
    ubyte rl8(ubyte val);
    /// rotate A left through carry bit
    void rla8();
    /// rotate right through carry bit
    ubyte rr8(ubyte val);
    /// rotate A right through carry bit
    void rra8();
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
    /// same as BIT, but take undocumented YF|XF flags from undocumented WZ register
    void ibit(ubyte val, ubyte mask);

    /// fetch an opcode byte and increment R register
    ubyte fetch_op();
    /// execute a single instruction, return number of cycles
    uint32_t step();

    #if YAKC_Z80_DECODER
    /// decode CB prefix instruction
    uint32_t do_cb(ubyte op, bool ext, int off);
    /// decode ED prefix instruction
    uint32_t do_ed(ubyte op);
    /// decode main instruction
    uint32_t do_op(ubyte op, bool ext);
    #else
    /// top-level opcode decoder (generated)
    uint32_t do_op();
    #endif
};

//------------------------------------------------------------------------------
inline z80::z80() :
BC(0), DE(0), HL(0), AF(0), IX(0), IY(0), WZ(0), SP(0), 
BC_(0), DE_(0), HL_(0), AF_(0), WZ_(0),
PC(0), I(0), R(0), IM(0),
HALT(false), IFF1(false), IFF2(false), INV(false),
in_func(nullptr),
out_func(nullptr),
inout_userdata(nullptr),
irq_device(nullptr),
irq_received(false),
enable_interrupt(false),
break_on_invalid_opcode(false) {
    this->init_tables();
}

//------------------------------------------------------------------------------
inline void
z80::init(cb_in func_in, cb_out func_out, void* userdata) {
    YAKC_ASSERT(func_in && func_out);
    this->init_tables();
    this->reset();
    this->in_func = func_in;
    this->out_func = func_out;
    this->inout_userdata = userdata;
}

//------------------------------------------------------------------------------
#define YAKC_SZ(val) ((val&0xFF)?(val&SF):ZF)
inline void
z80::init_tables() {
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

    #if YAKC_Z80_DECODER
    // 8-bit register index mapping table into R8[]
    r[0] = r2[0] = 1;   // B
    r[1] = r2[1] = 0;   // C
    r[2] = r2[2] = 3;   // D
    r[3] = r2[3] = 2;   // E
    r[4] = r2[4] = 5;   // H
    r[5] = r2[5] = 4;   // L
    r[6] = r2[6] = -1;  // (HL)
    r[7] = r2[7] = 7;   // A

    // 16-bit register index mapping table with SP into r16[]
    rp[0] = 0;  // BC
    rp[1] = 1;  // DE
    rp[2] = 2;  // HL
    rp[3] = 7;  // SP

    // 16-bit register index mapping table with AF into r16[]
    rp2[0] = 0; // BC
    rp2[1] = 1; // DE
    rp2[2] = 2; // HL
    rp2[3] = 3; // AF
    #endif
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
    this->PC = 0;
    this->WZ = this->PC;
    this->IM = 0;
    this->HALT = false;
    this->IFF1 = false;
    this->IFF2 = false;
    this->I = 0;
    this->R = 0;
    this->irq_received = false;
    this->enable_interrupt = false;
}

//------------------------------------------------------------------------------
inline uint32_t
z80::invalid_opcode(uword opsize) {
    if (this->break_on_invalid_opcode) {
        INV = true;
        PC -= opsize;     // stuck on invalid opcode
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
    ubyte undoc = ~(YF|XF);
    return (F & undoc) == expected;
}

//------------------------------------------------------------------------------
inline void
z80::halt() {
    HALT = true;
    PC--;
}

//------------------------------------------------------------------------------
inline void
z80::rst(ubyte vec) {
    SP -= 2;
    mem.w16(SP, PC);
    WZ = PC = (uword) vec;
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
        YAKC_ASSERT(2 == this->IM);

        // leave halt
        if (this->HALT) {
            this->HALT = false;
            this->PC++;
        }

        // NOTE: currently there's no timeout on an interrupt request if
        // interrupts are disabled for too long, immediately cancelling
        // the request doesn't work right (has problems with some KC
        // games which have high-frequency interrupts for playing sound)
        if (this->irq_device) {
            if (this->IFF1) {
                this->irq_received = false;
                // handle interrupt
                this->IFF1 = this->IFF2 = false;
                ubyte vec = this->irq_device->interrupt_acknowledged();
                uword addr = (this->I<<8)|(vec&0xFE);
                this->SP -= 2;
                this->mem.w16(this->SP, this->PC);
                this->PC = this->mem.r16(addr);
                tstates += 19;
            }
        }
        this->WZ = this->PC;
    }
    return tstates;
}

//------------------------------------------------------------------------------
inline void
z80::reti() {
    // this is the same as RET
    WZ = PC = mem.r16(SP);
    SP += 2;
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
    IFF1 = false;
    IFF2 = false;
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
        out_func(inout_userdata, port, val);
    }
}

//------------------------------------------------------------------------------
inline ubyte
z80::sziff2(ubyte val, bool iff2) {
    ubyte f = YAKC_SZ(val);
    f |= (val & (YF|XF));
    if (iff2) f |= PF;
    return f;
}

//------------------------------------------------------------------------------
inline void
z80::add8(ubyte add) {
    int r = A + add;
    ubyte f = YAKC_SZ(r);
    f |= r&(YF|XF);
    if (r > 0xFF) f |= CF;
    if ((r & 0xF) < (A & 0xF)) f |= HF;
    if (((A & 0x80) == (add & 0x80)) && ((r & 0x80) != (A & 0x80))) f |= VF;
    F = f;
    A = ubyte(r);
}

//------------------------------------------------------------------------------
inline void
z80::adc8(ubyte add) {
    if (F & CF) {
        int r = A + add + 1;
        ubyte f = YAKC_SZ(r);
        f |= r&(YF|XF);
        if (r > 0xFF) f |= CF;
        if ((r & 0xF) <= (A & 0xF)) f |= HF;
        if (((A&0x80) == (add&0x80)) && ((r&0x80) != (A&0x80))) f |= VF;
        F = f;
        A = ubyte(r);
    }
    else {
        add8(add);
    }
}

//------------------------------------------------------------------------------
inline void
z80::sub8(ubyte sub) {
    int r = int(A) - int(sub);
    ubyte f = NF | YAKC_SZ(r);
    f |= r&(YF|XF);
    if (r < 0) f |= CF;
    if ((r & 0xF) > (A & 0xF)) f |= HF;
    if (((A&0x80) != (sub&0x80)) && ((r&0x80) != (A&0x80))) f |= VF;
    F = f;
    A = ubyte(r);
}

//------------------------------------------------------------------------------
inline void
z80::cp8(ubyte sub) {
    // NOTE: XF|YF are set from sub, not from result!
    int r = int(A) - int(sub);
    ubyte f = NF | YAKC_SZ(r);
    f |= sub&(YF|XF);
    if (r < 0) f |= CF;
    if ((r & 0xF) > (A & 0xF)) f |= HF;
    if (((A&0x80) != (sub&0x80)) && ((r&0x80) != (A&0x80))) f |= VF;
    F = f;
}

//------------------------------------------------------------------------------
inline void
z80::neg8() {
    int r = -int(A);
    ubyte f = NF | YAKC_SZ(r);
    f |= r&(YF|XF);
    if (r < 0) f |= CF;
    if ((r & 0xF) > 0) f |= HF;
    if ((0 != (A&0x80)) && ((r&0x80) != 0)) f |= VF;
    F = f;
    A = ubyte(r);
}

//------------------------------------------------------------------------------
inline void
z80::sbc8(ubyte sub) {
    if (F & CF) {
        // don't waste flag table space for rarely used instructions
        int r = int(A) - int(sub) - 1;
        ubyte f = NF | YAKC_SZ(r);
        f |= r&(YF|XF);
        if (r < 0) f |= CF;
        if ((r & 0xF) >= (A & 0xF)) f |= HF;
        if (((A&0x80) != (sub&0x80)) && ((r&0x80) != (A&0x80))) f |= VF;
        F = f;
        A = ubyte(r);
    }
    else {
        sub8(sub);
    }
}

//------------------------------------------------------------------------------
inline void
z80::and8(ubyte val) {
    A &= val;
    F = szp[A]|HF;
}

//------------------------------------------------------------------------------
inline void
z80::or8(ubyte val) {
    A |= val;
    F = szp[A];
}

//------------------------------------------------------------------------------
inline void
z80::xor8(ubyte val) {
    A ^= val;
    F = szp[A];
}

//------------------------------------------------------------------------------
inline ubyte
z80::inc8(ubyte val) {
    ubyte r = val + 1;
    ubyte f = YAKC_SZ(r);
    f |= r&(XF|YF);
    if ((r & 0xF) == 0) f |= HF;
    if (r == 0x80) f |= VF;
    F = f | (F & CF);
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::dec8(ubyte val) {
    ubyte r = val - 1;
    ubyte f = NF | YAKC_SZ(r);
    f |= r&(XF|YF);    
    if ((r & 0xF) == 0xF) f |= HF;
    if (r == 0x7F) f |= VF;
    F = f | (F & CF);
    return r;
}

//------------------------------------------------------------------------------
inline uword
z80::add16(uword acc, uword val) {
    WZ = acc+1;
    uint32_t res = acc + val;
    // flag computation taken from MAME
    F = (F & (SF|ZF|VF)) |
        (((acc^res^val)>>8)&HF)|
        ((res>>16) & CF) | ((res >> 8) & (YF|XF));
    return (uword)res;
}

//------------------------------------------------------------------------------
inline uword
z80::adc16(uword acc, uword val) {
    WZ = acc+1;
    uint32_t res = acc + val + (F & CF);
    // flag computation taken from MAME
    F = (((acc^res^val)>>8)&HF) |
        ((res>>16)&CF) |
        ((res>>8)&(SF|YF|XF)) |
        ((res & 0xFFFF) ? 0 : ZF) |
        (((val^acc^0x8000) & (val^res)&0x8000)>>13);
    return res;
}

//------------------------------------------------------------------------------
inline uword
z80::sbc16(uword acc, uword val) {
    WZ = acc+1;
    uint32_t res = acc - val - (F & CF);
    // flag computation taken from MAME
    F = (((acc^res^val)>>8)&HF) | NF |
        ((res>>16)&CF) |
        ((res>>8) & (SF|YF|XF)) |
        ((res & 0xFFFF) ? 0 : ZF) |
        (((val^acc) & (acc^res)&0x8000)>>13);
    return res;
}

//------------------------------------------------------------------------------
inline void
z80::ldi() {
    ubyte val = mem.r8(HL);
    mem.w8(DE, val);
    val += A;
    ubyte f = F & (SF|ZF|CF);
    if (val & 0x02) f |= YF;
    if (val & 0x08) f |= XF;
    HL++;
    DE++;
    BC--;
    if (BC) {
        f |= VF;
    }
    F = f;
}

//------------------------------------------------------------------------------
inline int
z80::ldir() {
    ldi();
    if (BC != 0) {
        PC -= 2;
        WZ = PC + 1;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80::ldd() {
    ubyte val = mem.r8(HL);
    mem.w8(DE, val);
    val += A;
    ubyte f = F & (SF|ZF|CF);
    if (val & 0x02) f |= YF;
    if (val & 0x08) f |= XF;
    HL--;
    DE--;
    BC--;
    if (BC) {
        f |= VF;
    }
    F = f;
}

//------------------------------------------------------------------------------
inline int
z80::lddr() {
    ldd();
    if (BC != 0) {
        PC -= 2;
        WZ = PC + 1;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80::cpi() {
    int r = int(A) - int(mem.r8(HL));
    ubyte f = NF | (F & CF) | YAKC_SZ(r);
    if ((r & 0xF) > (A & 0xF)) {
        f |= HF;
        r--;
    }
    if (r & 0x02) f |= YF;
    if (r & 0x08) f |= XF;
    WZ++;
    HL++;
    BC--;
    if (BC) {
        f |= VF;
    }
    F = f;
}

//------------------------------------------------------------------------------
inline int
z80::cpir() {
    cpi();
    if ((BC != 0) && !(F & ZF)) {
        PC -= 2;
        WZ = PC + 1;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80::cpd() {
    int r = int(A) - int(mem.r8(HL));
    ubyte f = NF | (F & CF) | YAKC_SZ(r);
    if ((r & 0xF) > (A & 0xF)) {
        f |= HF;
        r--;
    }
    if (r & 0x02) f |= YF;
    if (r & 0x08) f |= XF;
    WZ--;
    HL--;
    BC--;
    if (BC) {
        f |= VF;
    }
    F = f;
}

//------------------------------------------------------------------------------
inline int
z80::cpdr() {
    cpd();
    if ((BC != 0) && !(F & ZF)) {
        PC -= 2;
        WZ = PC + 1;
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
    ubyte f = B ? B & SF : ZF;
    if (io_val & SF) f |= NF;
    uint32_t t = (uint32_t)((C+c_add)&0xFF) + (uint32_t)io_val;
    if (t & 0x100) f |= HF|CF;
    f |= szp[ubyte(t & 0x07)^B] & PF;
    return f;
}

//------------------------------------------------------------------------------
inline void
z80::ini() {
    ubyte io_val = in(BC);
    WZ = BC + 1;
    B--;
    mem.w8(HL++, io_val);
    F = ini_ind_flags(io_val, +1);
}

//------------------------------------------------------------------------------
inline int
z80::inir() {
    ini();
    if (B != 0) {
        PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80::ind() {
    ubyte io_val = in(BC);
    WZ = BC - 1;
    B--;
    mem.w8(HL--, io_val);
    F = ini_ind_flags(io_val, -1);
}

//------------------------------------------------------------------------------
inline int
z80::indr() {
    ind();
    if (B != 0) {
        PC -= 2;
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
    ubyte f = B ? B & SF : ZF;
    if (io_val & SF) f |= NF;
    uint32_t t = (uint32_t)L + (uint32_t)io_val;
    if (t & 0x100) f |= HF|CF;
    f |= szp[ubyte(t & 0x07)^B] & PF;
    return f;
}

//------------------------------------------------------------------------------
inline void
z80::outi() {
    ubyte io_val = mem.r8(HL++);
    B--;
    WZ = BC + 1;
    out(BC, io_val);
    F = outi_outd_flags(io_val);
}

//------------------------------------------------------------------------------
inline int
z80::otir() {
    outi();
    if (B != 0) {
        PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80::outd() {
    ubyte io_val = mem.r8(HL--);
    B--;
    WZ = BC - 1;
    out(BC, io_val);
    F = outi_outd_flags(io_val);
}

//------------------------------------------------------------------------------
inline int
z80::otdr() {
    outd();
    if (B != 0) {
        PC -= 2;
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
    ubyte val = A;
    if (F & NF) {
        if (((A & 0xF) > 0x9) || (F & HF)) {
            val -= 0x06;
        }
        if ((A > 0x99) || (F & CF)) {
            val -= 0x60;
        }
    }
    else {
        if (((A & 0xF) > 0x9) || (F & HF)) {
            val += 0x06;
        }
        if ((A > 0x99) || (F & CF)) {
            val += 0x60;
        }
    }
    F &= CF|NF;
    F |= (A > 0x99) ? CF:0;
    F |= (A^val) & HF;
    F |= szp[val];
    A = val;
}

//------------------------------------------------------------------------------
inline ubyte
z80::rlc8(ubyte val) {
    ubyte r = val<<1|val>>7;
    F = szp[r] | ((val & 0x80) ? CF : 0);
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::rlca8() {
    ubyte r = A<<1|A>>7;
    F = ((A & 0x80) ? CF : 0) | (F & (SF|ZF|PF)) | (r&(XF|YF));
    A = r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::rrc8(ubyte val) {
    ubyte r = val>>1|val<<7;
    F = szp[r] | (val & CF);
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::rrca8() {
    ubyte r = A>>1|A<<7;
    F = (A & CF) | (F & (SF|ZF|PF)) | (r&(YF|XF));
    A = r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::rl8(ubyte val) {
    ubyte r = val<<1 | (F & CF);
    F = (val & 0x80 ? CF : 0) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::rla8() {
    ubyte r = A<<1 | (F & CF);
    F = (A & 0x80 ? CF : 0) | (F & (SF|ZF|PF)) | (r&(YF|XF));
    A = r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::rr8(ubyte val) {
    ubyte r = val>>1 | ((F & CF) ? 0x80:0x00);
    F = (val & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::rra8() {
    ubyte r = A>>1 | ((F & CF)<<7);
    F = (A & CF) | (F & (SF|ZF|PF)) | (r&(YF|XF));
    A = r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::sla8(ubyte val) {
    ubyte r = val<<1;
    F = (val & 0x80 ? CF : 0) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::sll8(ubyte val) {
    // undocument! sll8 is identical with sla8 but inserts a 1 into the LSB
    ubyte r = (val<<1) | 1;
    F = (val & 0x80 ? CF : 0) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::sra8(ubyte val) {
    ubyte r = (val>>1) | (val & 0x80);
    F = (val & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::srl8(ubyte val) {
    ubyte r = val>>1;
    F = (val & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::rld() {
    WZ = HL;
    ubyte x = mem.r8(WZ++);
    ubyte tmp = A & 0xF;              // store A low nibble
    A = (A & 0xF0) | (x>>4);    // move (HL) high nibble into A low nibble
    x = (x<<4) | tmp;   // move (HL) low to high nibble, move A low nibble to (HL) low nibble
    mem.w8(HL, x);
    F = szp[A] | (F & CF);
}

//------------------------------------------------------------------------------
inline void
z80::rrd() {
    WZ = HL;
    ubyte x = mem.r8(WZ++);
    ubyte tmp = A & 0xF;                  // store A low nibble
    A = (A & 0xF0) | (x & 0x0F);    // move (HL) low nibble to A low nibble
    x = (x >> 4) | (tmp << 4);  // move A low nibble to (HL) high nibble, and (HL) high nibble to (HL) low nibble
    mem.w8(HL, x);
    F = szp[A] | (F & CF);
}

//------------------------------------------------------------------------------
inline void
z80::bit(ubyte val, ubyte mask) {
    ubyte r = val & mask;
    ubyte f = HF | (r ? (r & SF) : (ZF|PF));
    f |= (val & (YF|XF));
    F = f | (F & CF);
}

//------------------------------------------------------------------------------
inline void
z80::ibit(ubyte val, ubyte mask) {
    // this is the version for the BIT instruction for (HL), (IX+d), (IY+d),
    // these set the undocumented YF and XF flags from high byte of HL+1
    // or IX/IY+d
    ubyte r = val & mask;
    ubyte f = HF | (r ? (r & SF) : (ZF|PF));
    f |= (W & (YF|XF));
    F = f | (F & CF);
}

//------------------------------------------------------------------------------
inline ubyte
z80::fetch_op() {
    R = (R + 1) & 0x7F;
    return mem.r8(PC++);
}

//------------------------------------------------------------------------------
inline uint32_t
z80::step() {
    INV = false;
    if (enable_interrupt) {
        IFF1 = IFF2 = true;
        enable_interrupt = false;
    }
    #if YAKC_Z80_DECODER
    return do_op(fetch_op(), false);
    #else
    return do_op();
    #endif
}

} // namespace
#if YAKC_Z80_DECODER
#include "core/z80_decoder.h"
#else
#include "core/z80_opcodes.h"
#endif
