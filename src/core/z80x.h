#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::z80
    @brief alternative z80 implementation
*/
#include "core/common.h"
#include "core/memory.h"
#include "core/z80int.h"

namespace yakc {

class z80x {
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
        ubyte R8[14];       // C, B, E, D, L, H, F, A, IXL, IXH, IYL, IYH, SP
        uword R16[7];       // BC, DE, HL, AF, IX, IY, SP
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
            uword SP;
        };
    };

    /// shadow register set
    uword BC_;
    uword DE_;
    uword HL_;
    uword AF_;
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

    /// 8-bit register index map into R8
    int r[8];
    /// 16-bit register index map with SP into R16
    int rp[4];
    /// 16-bit register index map with AF into R16
    int rp2[4];

    /// access to HL/IX/IY 16-bit register slot
    #define HLIXIY R16[rp[2]]

    /// memory map
    memory mem;

    /// flag lookup table for SZP flag combinations
    ubyte szp[256];

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
    z80x();
    /// one-time init
    void init(cb_in func_in, cb_out func_out, void* userdata);
    /// initialize lookup tables
    void init_tables();
    /// connect the highest priority interrupt controller device
    void connect_irq_device(z80int* device);
    /// perform a reset (RESET pin triggered)
    void reset();
    /// helper to test expected flag bitmask
    bool test_flags(ubyte expected) const;
    /// called when invalid opcode has been hit
    uint32_t invalid_opcode(uword opsize);
    /// check flag status for RET cc, JP cc, CALL cc, etc...
    bool cc(ubyte y) const;
    /// conditionally load d offset for (IX/IY+d) instructions
    int d(bool load_d);
    /// get address for (HL), (IX+d), (IY+d)
    uword iHLIXIYd(bool ext);
    /// helper method to swap 2 16-bit registers
    static void swap16(uword& r0, uword& r1);

    /// halt instruction
    void halt();
    /// implement the EI instruction
    void ei();
    /// implement the DI instruction
    void di();
    /// call in-handler, return result
    ubyte in(uword port);
    /// call out-handler
    void out(uword port, ubyte val);    
    /// rst instruction (vec is 0x00, 0x08, 0x10, ...)
    void rst(ubyte vec);
    /// perform a general ALU op (add, adc, sub, sbc, cp, and, or, xor)
    void alu8(ubyte alu, ubyte val);

    /// perform an 8-bit add and update flags
    void add8(ubyte add);
    /// perform an 8-bit adc and update flags
    void adc8(ubyte add);
    /// compute flags for sub8 operation (also cp8 and neg8)
    ubyte sub8_flags(ubyte acc, ubyte sub);
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
    /// implement the DAA instruction
    void daa();
    /// implements the BIT test instruction, updates flags
    void bit(ubyte val, ubyte mask);

    /// perform an 16-bit add, update flags and return result
    uword add16(uword acc, uword val);
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

    /// fetch an opcode byte and increment R register
    ubyte fetch_op();
    /// decode CB prefix instruction
    uint32_t do_cb(ubyte op, bool ext, int off);
    /// decode ED prefix instruction
    uint32_t do_ed(ubyte op);
    /// decode main instruction
    uint32_t do_op(ubyte op, bool ext);
    /// execute a single instruction, return number of cycles
    uint32_t step();
};

#define YAKC_SZ(val) ((val&0xFF)?(val&SF):ZF)

//------------------------------------------------------------------------------
inline
z80x::z80x() :
BC(0), DE(0), HL(0), AF(0), IX(0), IY(0), SP(0),
BC_(0), DE_(0), HL_(0), AF_(0),
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
z80x::init(cb_in func_in, cb_out func_out, void* userdata) {
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
z80x::init_tables() {
    // SZP flag lookup tables
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

    // 8-bit register index mapping table into R8[]
    r[0] = 1;   // B
    r[1] = 0;   // C
    r[2] = 3;   // D
    r[3] = 2;   // E
    r[4] = 5;   // H
    r[5] = 4;   // L
    r[6] = -1;  // (HL)
    r[7] = 7;   // A

    // 16-bit register index mapping table with SP into r16[]
    rp[0] = 0;  // BC
    rp[1] = 1;  // DE
    rp[2] = 2;  // HL
    rp[3] = 6;  // SP

    // 16-bit register index mapping table with AF into r16[]
    rp2[0] = 0; // BC
    rp2[1] = 1; // DE
    rp2[2] = 2; // HL
    rp2[3] = 3; // AF
}

//------------------------------------------------------------------------------
inline void
z80x::connect_irq_device(z80int* device) {
    YAKC_ASSERT(device);
    this->irq_device = device;
}

//------------------------------------------------------------------------------
inline void
z80x::reset() {
    this->PC = 0;
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
inline bool
z80x::test_flags(ubyte expected) const {
    // mask out undocumented flags
    ubyte undoc = ~(XF|YF);
    return (F & undoc) == expected;
}

//------------------------------------------------------------------------------
inline uint32_t
z80x::invalid_opcode(uword opsize) {
    if (this->break_on_invalid_opcode) {
        INV = true;
        PC -= opsize;     // stuck on invalid opcode
    }
    return 4;
}

//------------------------------------------------------------------------------
inline bool
z80x::cc(ubyte y) const {
    switch (y) {
        case 0: return !(F & ZF);  // NZ
        case 1: return F & ZF;     // Z
        case 2: return !(F & CF);  // NC
        case 3: return F & CF;     // C
        case 4: return !(F & PF);  // PO
        case 5: return F & PF;     // PE
        case 6: return !(F & SF);  // P
        default: return F & SF;    // M
    }
}

//------------------------------------------------------------------------------
inline int
z80x::d(bool ext) {
    if (ext) {
        return mem.rs8(PC++);
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
inline uword
z80x::iHLIXIYd(bool ext) {
    return HLIXIY+d(ext);
}

//------------------------------------------------------------------------------
inline void
z80x::swap16(uword& r0, uword& r1) {
    uword tmp = r0;
    r0 = r1;
    r1 = tmp;
}

//------------------------------------------------------------------------------
inline void
z80x::halt() {
    HALT = true;
    PC--;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::in(uword port) {
    if (in_func) {
        return in_func(inout_userdata, port);
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
inline void
z80x::out(uword port, ubyte val) {
    if (out_func) {
        out_func(inout_userdata, port, val);
    }
}

//------------------------------------------------------------------------------
inline void
z80x::ei() {
    // NOTE: interrupts are actually enabled *after the next instruction*,
    // thus we only set a flag here, the IFF flags will then be changed
    // during the next instruction decode in step()
    this->enable_interrupt = true;
}

//------------------------------------------------------------------------------
inline void
z80x::di() {
    this->IFF1 = false;
    this->IFF2 = false;
}

//------------------------------------------------------------------------------
inline void
z80x::add8(ubyte add) {
    int r = A + add;
    ubyte f = YAKC_SZ(r);
    if (r > 0xFF) f |= CF;
    if ((r & 0xF) < (A & 0xF)) f |= HF;
    if (((A & 0x80) == (add & 0x80)) && ((r & 0x80) != (A & 0x80))) f |= VF;
    F = f;
    A = ubyte(r);
}

//------------------------------------------------------------------------------
inline void
z80x::adc8(ubyte add) {
    if (F & CF) {
        int r = int(A) + int(add) + 1;
        ubyte f = YAKC_SZ(r);
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
inline ubyte
z80x::sub8_flags(ubyte acc, ubyte sub) {
    int r = int(acc) - int(sub);
    ubyte f = NF | YAKC_SZ(r);
    if (r < 0) f |= CF;
    if ((r & 0xF) > (acc & 0xF)) f |= HF;
    if (((acc&0x80) != (sub&0x80)) && ((r&0x80) != (acc&0x80))) f |= VF;
    return f;
}

//------------------------------------------------------------------------------
inline void
z80x::sub8(ubyte sub) {
    F = sub8_flags(A, sub);
    A -= sub;
}

//------------------------------------------------------------------------------
inline void
z80x::cp8(ubyte sub) {
    F = sub8_flags(A, sub);
}

//------------------------------------------------------------------------------
inline void
z80x::neg8() {
    F = sub8_flags(0, A);
    A = ubyte(0) - A;
}

//------------------------------------------------------------------------------
inline void
z80x::sbc8(ubyte sub) {
    if (F & CF) {
        int r = int(A) - int(sub) - 1;
        ubyte f = NF | YAKC_SZ(r);
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
z80x::and8(ubyte val) {
    A &= val;
    F = szp[A]|HF;
}

//------------------------------------------------------------------------------
inline void
z80x::or8(ubyte val) {
    A |= val;
    F = szp[A];
}

//------------------------------------------------------------------------------
inline void
z80x::xor8(ubyte val) {
    A ^= val;
    F = szp[A];
}

//------------------------------------------------------------------------------
inline ubyte
z80x::inc8(ubyte val) {
    ubyte r = val + 1;
    ubyte f = YAKC_SZ(r);
    if ((r & 0xF) == 0) f |= HF;
    if (r == 0x80) f |= VF;
    F = f | (F & CF);
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::dec8(ubyte val) {
    ubyte r = val - 1;
    ubyte f = NF | YAKC_SZ(r);
    if ((r & 0xF) == 0xF) f |= HF;
    if (r == 0x7F) f |= VF;
    F = f | (F & CF);
    return r;
}

//------------------------------------------------------------------------------
inline void
z80x::rst(ubyte vec) {
    SP -= 2;
    mem.w16(SP, PC);
    PC = (uword) vec;
}

//------------------------------------------------------------------------------
inline void
z80x::alu8(ubyte alu, ubyte val) {
    switch (alu) {
        case 0: add8(val); break;
        case 1: adc8(val); break;
        case 2: sub8(val); break;
        case 3: sbc8(val); break;
        case 4: and8(val); break;
        case 5: xor8(val); break;
        case 6: or8(val); break;
        case 7: cp8(val); break;
    }
}

//------------------------------------------------------------------------------
inline ubyte
z80x::rlc8(ubyte val, bool flags_szp) {
    ubyte r = val<<1|val>>7;
    ubyte f = (val & 0x80) ? CF : 0;
    if (flags_szp) {
        F = f | szp[r];
    }
    else {
        F = f | (F & (SF|ZF|PF));
    }
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::rrc8(ubyte val, bool flags_szp) {
    ubyte r = val>>1|val<<7;
    ubyte f = val & CF;
    if (flags_szp) {
        F = f | szp[r];
    }
    else {
        F = f | (F & (SF|ZF|PF));
    }
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::rl8(ubyte val, bool flags_szp) {
    ubyte r = val<<1 | ((F & CF) ? 0x01:0x00);
    ubyte f = val & 0x80 ? CF : 0;
    if (flags_szp) {
        F = f | szp[r];
    }
    else {
        F = f | (F & (SF|ZF|PF));
    }
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::rr8(ubyte val, bool flags_szp) {
    ubyte r = val>>1 | ((F & CF) ? 0x80:0x00);
    ubyte f = val & CF;
    if (flags_szp) {
        F = f | szp[r];
    }
    else {
        F = f | (F & (SF|ZF|PF));
    }
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::sla8(ubyte val) {
    ubyte r = val<<1;
    F = (val & 0x80 ? CF : 0) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::sll8(ubyte val) {
    // undocument! sll8 is identical with sla8 but inserts a 1 into the LSB
    ubyte r = (val<<1) | 1;
    F = (val & 0x80 ? CF : 0) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::sra8(ubyte val) {
    ubyte r = (val>>1) | (val & 0x80);
    F = (val & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::srl8(ubyte val) {
    ubyte r = val>>1;
    F = (val & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline void
z80x::rld() {
    ubyte x = mem.r8(HL);
    ubyte tmp = A & 0xF;        // store A low nibble
    A = (A & 0xF0) | (x>>4);    // move (HL) high nibble into A low nibble
    x = (x<<4) | tmp;           // move (HL) low to high nibble, move A low nibble to (HL) low nibble
    mem.w8(HL, x);
    F = szp[A] | (F & CF);
}

//------------------------------------------------------------------------------
inline void
z80x::rrd() {
    ubyte x = mem.r8(HL);
    ubyte tmp = A & 0xF;            // store A low nibble
    A = (A & 0xF0) | (x & 0x0F);    // move (HL) low nibble to A low nibble
    x = (x >> 4) | (tmp << 4);      // move A low nibble to (HL) high nibble, and (HL) high nibble to (HL) low nibble
    mem.w8(HL, x);
    F = szp[A] | (F & CF);
}

//------------------------------------------------------------------------------
inline void
z80x::daa() {
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
inline void
z80x::bit(ubyte val, ubyte mask) {
    ubyte r = val & mask;
    ubyte f = HF | (r ? (r & SF) : (ZF|PF));
    f |= (val & (YF|XF));
    F = f | (F & CF);
}

//------------------------------------------------------------------------------
inline uword
z80x::add16(uword acc, uword val) {
    unsigned int res = acc + val;
    // flag computation taken from MAME
    F = (F & (SF|ZF|VF)) |
        (((acc^res^val)>>8)&HF)|
        ((res>>16) & CF) | ((res >> 8) & (YF|XF));
    return (uword)res;
}

//------------------------------------------------------------------------------
inline void
z80x::ldi() {
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
z80x::ldir() {
    ldi();
    if (BC) {
        PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80x::ldd() {
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
z80x::lddr() {
    ldd();
    if (BC) {
        PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80x::cpi() {
    int r = int(A) - int(mem.r8(HL));
    ubyte f = NF | (F & CF) | YAKC_SZ(r);
    if ((r & 0xF) > (A & 0xF)) {
        f |= HF;
        r--;
    }
    if (r & 0x02) f |= YF;
    if (r & 0x08) f |= XF;
    HL++;
    BC--;
    if (BC) {
        f |= VF;
    }
    F = f;
}

//------------------------------------------------------------------------------
inline int
z80x::cpir() {
    cpi();
    if (BC && !(F & ZF)) {
        PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline void
z80x::cpd() {
    int r = int(A) - int(mem.r8(HL));
    ubyte f = NF | (F & CF) | YAKC_SZ(r);
    if ((r & 0xF) > (A & 0xF)) {
        f |= HF;
        r--;
    }
    if (r & 0x02) f |= YF;
    if (r & 0x08) f |= XF;
    HL--;
    BC--;
    if (BC) {
        f |= VF;
    }
    F = f;
}

//------------------------------------------------------------------------------
inline int
z80x::cpdr() {
    cpd();
    if (BC && !(F & ZF)) {
        PC -= 2;
        return 21;
    }
    else {
        return 16;
    }
}

//------------------------------------------------------------------------------
inline ubyte
z80x::fetch_op() {
    R = (R + 1) & 0x7F;
    return mem.r8(PC++);
}

//------------------------------------------------------------------------------
inline uint32_t
z80x::do_cb(ubyte op, bool ext, int off) {
    const ubyte x = op>>7;
    const ubyte y = (op>>3) & 7;
    const ubyte z = op & 7;
    uint32_t ext_cycles = ext ? 4 : 0;
    if (x == 0) {
        // roll and shift ops
        ubyte val;
        uword addr;
        if (z == 6) {
            addr = HLIXIY+off;
            val = mem.r8(addr);
        }
        else {
            val = R8[r[z]];
        }
        switch (y) {
            case 0:
                //--- RLC
                val = rlc8(val, true);
                break;
            case 1:
                //--- RRC
                val = rrc8(val, true);
                break;
            case 2:
                //--- RL
                val = rl8(val, true);
                break;
            case 3:
                //--- RR
                val = rr8(val, true);
                break;
            case 4:
                //--- SLA
                val = sla8(val);
                break;
            case 5:
                //--- SRA
                val = sra8(val);
                break;
            case 6:
                //--- SLL
                val = sll8(val);
                break;
            case 7:
                //--- SRL
                val = srl8(val);
                break;
        }
        if (z == 6) {
            mem.w8(addr, val);
            return 15 + ext_cycles;
        }
        else {
            R8[r[z]] = val;
            return 8;
        }
    }
    else if (x == 1) {
        // BIT y,r[z]
        if (z == 6) {
            const uword addr = HLIXIY+off;
            bit(mem.r8(addr), 1<<y);
            return 12 + ext_cycles;
        }
        else {
            bit(R8[r[z]], 1<<y);
            return 8;
        }
    }
    else if (x == 2) {
        // RES y,r[z]
        if (z == 6) {
            const uword addr = HLIXIY+off;
            mem.w8(addr, mem.r8(addr) & ~(1<<y));
            return 15 + ext_cycles;
        }
        else {
            R8[r[z]] &= ~(1<<y);
            return 8;
        }
    }
    else if (x == 3) {
        // SET y,r[z]
        if (z == 6) {
            const uword addr = HLIXIY+off;
            mem.w8(addr, mem.r8(addr) | (1<<y));
            return 15 + ext_cycles;
        }
        else {
            R8[r[z]] |= (1<<y);
            return 8;
        }
    }
    YAKC_ASSERT(false);
}

//------------------------------------------------------------------------------
inline uint32_t
z80x::do_ed(ubyte op) {
    const ubyte x = op>>6;
    const ubyte y = (op>>3) & 7;
    const ubyte z = op & 7;
    if (x == 1) {
        switch (z) {
            //--- IN
            case 0: break;
            //-- OUT
            case 1: break;
            //--- SBC/ADC HL
            case 2: break;
            //--- load store 16-bit with immediate address
            case 3: break;
            //--- NEG
            case 4: break;
            //--- RETN, RETI
            case 5: break;
            //--- IM
            case 6: break;
            //--- assorted ops
            case 7: break;
        }
    }
    else if (x == 2) {
        //--- block instructions
        switch (z) {
            //--- LDI,LDD,LDIR,LDDR
            case 0:
                switch (y) {
                    case 4: ldi(); return 16;
                    case 5: ldd(); return 16;
                    case 6: return ldir();
                    case 7: return lddr();
                }
                break;
            //--- CPI,CPD,CPID,CPDR
            case 1:
                switch (y) {
                    case 4: cpi(); return 16;
                    case 5: cpd(); return 16;
                    case 6: return cpir();
                    case 7: return cpdr();
                }
                break;
            //--- INI,IND,INID,INDR
            case 2: break;
            //--- OUTI,OUTD,OTIR,OTDR
            case 3: break;
            default:
                return invalid_opcode(2);
        }
    }
    else {
        return invalid_opcode(2);
    }
    YAKC_ASSERT(false);
}

//------------------------------------------------------------------------------
inline uint32_t
z80x::do_op(ubyte op, bool ext) {
    const ubyte x = op>>6;
    const ubyte y = (op>>3) & 7;
    const ubyte z = op & 7;
    uint32_t ext_cycles = ext ? 8 : 0;
    if (x == 1) {
        // 8-bit load, or special case HALT for LD (HL),(HL)
        if (y == 6) {
            if (z == 6) {
                halt();
                return 4;
            }
            else {
                // LD (HL),r; LD (IX+d),r; LD (IY+d),r
                mem.w8(iHLIXIYd(ext), R8[r[z]]);
                return 7 + ext_cycles;
            }
        }
        else if (z == 6) {
            // LD r,(HL); LD r,(IX+d); LD r,(IY+d)
            R8[r[y]] = mem.r8(iHLIXIYd(ext));
            return 7 + ext_cycles;
        }
        else {
            // LD r,s
            R8[r[y]] = R8[r[z]];
            return 4;
        }
    }
    else if (x == 2) {
        // 8-bit ALU instruction with register or (HL), (IX+d), (IY+d)
        if (z == 6) {
            alu8(y, mem.r8(iHLIXIYd(ext)));
            return 7 + ext_cycles;
        }
        else {
            alu8(y, R8[r[z]]);
            return 4;
        }
    }
    else if (x == 0) {
        const ubyte p = y >> 1;
        const ubyte q = y & 1;
        switch (z) {
            //--- relative jumps and assorted ops
            case 0:
                switch (y) {
                    //--- NOP
                    case 0:
                        return 4;
                    //--- EX AF,AF'
                    case 1:
                        swap16(AF, AF_);
                        return 4;
                    //--- DJNZ d
                    case 2:
                        if (--B > 0) {
                            PC += mem.rs8(PC) + 1;
                            return 13;
                        }
                        else {
                            PC++;
                            return 8;
                        }
                    //--- JR d
                    case 3:
                        PC += mem.rs8(PC) + 1;
                        return 12;
                    //--- JR cc,d
                    default:
                        if (cc(y-4)) {
                            PC += mem.rs8(PC) + 1;
                            return 12;
                        }
                        else {
                            PC++;
                            return 7;
                        }
                }
                break;

            //--- 16-bit immediate load, and ADD HL,rr
            case 1:
                if (q == 0) {
                    R16[rp[p]] = mem.r16(PC);
                    PC += 2;
                    return 10;
                }
                else {
                    // ADD HL,rr, ADD IX,rr, ADD IY,rr
                    HLIXIY = add16(HLIXIY, R16[rp[p]]);
                    return 11;
                }
                break;

            //--- indirect loads
            case 2:
                switch (y) {
                    case 0:
                        //--- LD (BC),A
                        mem.w8(BC, A);
                        return 7;
                    case 1:
                        //--- LD A,(BC)
                        A = mem.r8(BC);
                        return 7;
                    case 2:
                        //--- LD (DE),A
                        mem.w8(DE, A);
                        return 7;
                    case 3:
                        //--- LD A,(DE)
                        A = mem.r8(DE);
                        return 7;
                    case 4:
                        //--- LD (nn),HL; LD (nn),IX; LD (nn),IY
                        mem.w16(mem.r16(PC), HLIXIY);
                        PC += 2;
                        return 16;
                    case 5:
                        //--- LD HL,(nn), LD IX,(nn), LD IY,(nn)
                        HLIXIY = mem.r16(mem.r16(PC));
                        PC += 2;
                        return 16;
                    case 6:
                        //--- LD (nn),A
                        mem.w8(mem.r16(PC), A);
                        PC += 2;
                        return 13;
                    case 7:
                        //--- LD A,(nn)
                        A = mem.r8(mem.r16(PC));
                        PC += 2;
                        return 13;
                }
                break;

            //--- INC/DEC rp[p] (16-bit)
            case 3:
                if (q == 0) {
                    R16[rp[p]]++;
                }
                else {
                    R16[rp[p]]--;
                }
                return 6;

            //--- INC r[y] (8-bit)
            case 4:
                if (y == 6) {
                    // INC (HL), INC (IX+d), INC (IY+d)
                    const uword addr = iHLIXIYd(ext);
                    mem.w8(addr, inc8(mem.r8(addr)));
                    return 11 + ext_cycles;
                }
                else {
                    R8[r[y]] = inc8(R8[r[y]]);
                    return 4;
                }
                break;
                
            //--- DEC r[y] (8-bit)
            case 5:
                if (y == 6) {
                    // DEC (HL), INC (IX+d), INC (IY+d)
                    const uword addr = iHLIXIYd(ext);
                    mem.w8(addr, dec8(mem.r8(addr)));
                    return 11 + ext_cycles;
                }
                else {
                    R8[r[y]] = dec8(R8[r[y]]);
                    return 4;
                }
                break;

            //--- LD r[y],n
            case 6:
                if (y == 6) {
                    mem.w8(iHLIXIYd(ext), mem.r8(PC++));
                    return ext ? 15 : 10;
                }
                else {
                    R8[r[y]] = mem.r8(PC++);
                    return 7;
                }
                break;

            //--- assorted ops on accumulator and flags
            case 7:
                switch (y) {
                    case 0:
                        //--- RLCA
                        A = rlc8(A, false);
                        return 4;
                    case 1:
                        //--- RRCA
                        A = rrc8(A, false);
                        return 4;
                    case 2:
                        //--- RLA
                        A = rl8(A, false);
                        return 4;
                    case 3:
                        //--- RRA
                        A = rr8(A, false);
                        return 4;
                    case 4:
                        //--- DAA
                        daa();
                        return 4;
                    case 5:
                        //--- CPL
                        A ^= 0xFF;
                        F = (F&(SF|ZF|PF|CF)) | HF | NF | (A&(YF|XF));
                        return 4;
                    case 6:
                        //--- SCF
                        F = (F&(SF|ZF|YF|XF|PF))|CF|(A&(YF|XF));
                        return 4;
                    case 7:
                        //--- CCF
                        F = ((F&(SF|ZF|YF|XF|PF|CF))|((F&CF)<<4)|(A&(YF|XF)))^CF;   // CCF
                        return 4;
                }
        }
        YAKC_ASSERT(false);
    }
    else if (x == 3) {
        const ubyte p = y >> 1;
        const ubyte q = y & 1;
        switch (z) {
            //--- RET cc
            case 0:
                if (cc(y)) {
                    PC = mem.r16(SP);
                    SP += 2;
                    return 11;
                }
                return 5;

            //--- POP and various ops
            case 1:
                if (q == 0) {
                    // POP rp2[p] (BC=0, DE=1, HL,IX,IY=2, AF=3)
                    R16[rp2[p]] = mem.r16(SP);
                    SP += 2;
                    return 10;
                }
                else {
                    switch (p) {
                        case 0:
                            //--- RET
                            PC = mem.r16(SP);
                            SP += 2;
                            return 10;
                        case 1:
                            //--- EXX (swap BC,DE,HL)
                            swap16(BC, BC_);
                            swap16(DE, DE_);
                            swap16(HL, HL_);
                            return 4;
                        case 2:
                            //--- JP HL; JP IX; JP IY
                            PC = HLIXIY;
                            return 4;
                        case 3:
                            //--- LD SP,HL; LD SP,IX; LD SP,IY
                            SP = HLIXIY;
                            return 6;
                    }
                }
                break;

            //--- JP cc
            case 2:
                if (cc(y)) {
                    PC = mem.r16(PC);
                }
                else {
                    PC += 2;
                }
                return 10;

            //--- assorted ops
            case 3:
                switch (y) {
                    case 0:
                        //--- JP nn
                        PC = mem.r16(PC);
                        return 10;
                    case 1:
                        //--- CB prefix
                        {
                            const int off = d(ext);
                            const ubyte op = fetch_op();
                            return do_cb(op, ext, off);
                        }
                    case 2:
                        //--- OUT (n),A
                        out((A<<8)|mem.r8(PC++), A);
                        return 11;
                    case 3:
                        //--- IN A,(n)
                        A = in((A<<8)|(mem.r8(PC++)));
                        return 11;
                    case 4:
                        //--- EX (SP),HL; EX (SP),IX; EX (SP),IY
                        {
                            uword tmp = mem.r16(SP);
                            mem.w16(SP, HLIXIY);
                            HLIXIY = tmp;
                        }
                        return 19;
                    case 5:
                        //--- EX DE,HL
                        swap16(DE, HL);
                        return 4;
                    case 6:
                        //--- DI
                        di();
                        return 4;
                    case 7:
                        //--- EI
                        ei();
                        return 4;
                }
                break;

            //--- CALL cc
            case 4:
                if (cc(y)) {
                    SP -= 2;
                    mem.w16(SP, PC+2);
                    PC = mem.r16(PC);
                    return 17;
                }
                else {
                    PC += 2;
                    return 10;
                }
                break;

            //--- PUSH and various ops
            case 5:
                if (q == 0) {
                    // PUSH rp2[p] (BC=0, DE=1, HL=2, AF=3), IX, IY
                    SP -= 2;
                    mem.w16(SP, R16[rp2[p]]);
                    return 11;
                }
                else {
                    switch (p) {
                        case 0:
                            //--- CALL nn
                            SP -= 2;
                            mem.w16(SP, PC+2);
                            PC = mem.r16(PC);
                            return 17;
                        case 1:
                            //--- DD prefix
                            if (!ext) {
                                // patch register lookup tables (HL->IX) and call self
                                rp[2] = rp2[2] = 4;
                                uint32_t cycles = do_op(fetch_op(), true) + 4;
                                rp[2] = rp2[2] = 2;
                                return cycles;
                            }
                            else {
                                return invalid_opcode(2);
                            }
                        case 2:
                            //--- ED prefix
                            if (!ext) {
                                return do_ed(fetch_op());
                            }
                            else {
                                return invalid_opcode(2);
                            }
                        case 3:
                            // FD prefix
                            if (!ext) {
                                // patch register lookup tables (HL->IY) and call self
                                rp[2] = rp2[2] = 5;
                                uint32_t cycles = do_op(fetch_op(), true) + 4;
                                rp[2] = rp2[2] = 2;
                                return cycles;
                            }
                            else {
                                return invalid_opcode(2);
                            }
                    }
                }
                break;

            //--- 8-bit ALU with immediate operand
            case 6:
                alu8(y, mem.r8(PC++));
                return 7;

            //--- RST
            case 7:
                rst(y*8);
                return 11;
        }
    }
    YAKC_ASSERT(false);
}

//------------------------------------------------------------------------------
inline uint32_t
z80x::step() {
    // see: http://www.z80.info/decoding.htm
    INV = false;
    if (enable_interrupt) {
        IFF1 = IFF2 = true;
        enable_interrupt = false;
    }
    return do_op(fetch_op(), false);
}

} // namespace yakc

