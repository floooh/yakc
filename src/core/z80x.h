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
    /// register indices and flag bits
    enum {
        B = 0,
        C = 1,
        D = 2,
        E = 3,
        H = 4,
        L = 5,
        F = 6,
        A = 7,
        IXH = 8,
        IXL = 9,
        IYH = 10,
        IYL = 11,
        NUM_R8 = 12,

        BC = 0,
        DE = 1,
        HL = 2,
        AF = 3,
        IX = 4,
        IY = 5,
        NUM_R16 = 6,

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

    /// main register set
    ubyte REG[NUM_R8];
    /// shadow register set
    ubyte REG_[NUM_R8];
    /// stack pointer
    uword SP;
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
    /// initialize the flag lookup tables
    void init_flag_tables();
    /// connect the highest priority interrupt controller device
    void connect_irq_device(z80int* device);
    /// perform a reset (RESET pin triggered)
    void reset();
    /// helper to test expected flag bitmask
    bool test_flags(ubyte expected) const;
    /// called when invalid opcode has been hit
    uint32_t invalid_opcode(uword opsize);

    /// write a 16-bit register (index: BC=0, DE=1, HL=2, AF=3)
    void rw16(int index, uword value);
    /// read a 16-bit register (index: BC=0, DE=1, HL=2, AF=3)
    uword rr16(int index) const;
    /// read a 16-bit shadow register (index: BC=0, DE=1, HL=2, AF=3)
    uword rr16_(int index) const;

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
    /// check flag status for RET cc, JP cc, CALL cc, etc...
    bool cc(ubyte y) const;
    /// conditionally load d offset for (IX/IY+d) instructions
    int d(bool load_d);

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

    /// fetch an opcode byte and increment R register
    ubyte fetch_op();
    /// decode CB prefix instruction
    uint32_t do_cb(ubyte op, int hl, bool ext, int off);
    /// decode main instruction
    uint32_t do_op(ubyte op, int hl, bool ext);
    /// execute a single instruction, return number of cycles
    uint32_t step();
};

#define YAKC_SZ(val) ((val&0xFF)?(val&SF):ZF)

//------------------------------------------------------------------------------
inline
z80x::z80x() :
SP(0), PC(0), I(0), R(0), IM(0),
HALT(false), IFF1(false), IFF2(false), INV(false),
in_func(nullptr),
out_func(nullptr),
inout_userdata(nullptr),
irq_device(nullptr),
irq_received(false),
enable_interrupt(false),
break_on_invalid_opcode(false) {
    for (int i = 0; i < NUM_R8; i++) {
        REG[i] = 0;
        REG_[i] = 0;
    }
}

//------------------------------------------------------------------------------
inline void
z80x::init(cb_in func_in, cb_out func_out, void* userdata) {
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
z80x::init_flag_tables() {
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
    return (REG[F] & undoc) == expected;
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
inline void
z80x::rw16(int index, uword val) {
    if (AF == index) {
        // special case AF
        REG[index<<1] = val & 0xFF;
        REG[(index<<1)+1] = val>>8;
    }
    else {
        REG[index<<1] = val>>8;
        REG[(index<<1)+1] = val & 0xFF;
    }
}

//------------------------------------------------------------------------------
inline uword
z80x::rr16(int index) const {
    uword val;
    if (AF == index) {
        // special case AF
        val = REG[(index<<1)+1]<<8 | REG[index<<1];
    }
    else {
        val = REG[index<<1]<<8 | REG[(index<<1)+1];
    }
    return val;
}

//------------------------------------------------------------------------------
inline uword
z80x::rr16_(int index) const {
    uword val;
    if (AF == index) {
        // special case AF
        val = REG_[(index<<1)+1]<<8 | REG_[index<<1];
    }
    else {
        val = REG_[index<<1]<<8 | REG_[(index<<1)+1];
    }
    return val;
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
    int r = REG[A] + add;
    ubyte f = YAKC_SZ(r);
    if (r > 0xFF) f |= CF;
    if ((r & 0xF) < (REG[A] & 0xF)) f |= HF;
    if (((REG[A] & 0x80) == (add & 0x80)) && ((r & 0x80) != (REG[A] & 0x80))) f |= VF;
    REG[F] = f;
    REG[A] = ubyte(r);
}

//------------------------------------------------------------------------------
inline void
z80x::adc8(ubyte add) {
    if (REG[F] & CF) {
        int r = int(REG[A]) + int(add) + 1;
        ubyte f = YAKC_SZ(r);
        if (r > 0xFF) f |= CF;
        if ((r & 0xF) <= (REG[A] & 0xF)) f |= HF;
        if (((REG[A]&0x80) == (add&0x80)) && ((r&0x80) != (REG[A]&0x80))) f |= VF;
        REG[F] = f;
        REG[A] = ubyte(r);
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
    REG[F] = sub8_flags(REG[A], sub);
    REG[A] -= sub;
}

//------------------------------------------------------------------------------
inline void
z80x::cp8(ubyte sub) {
    REG[F] = sub8_flags(REG[A], sub);
}

//------------------------------------------------------------------------------
inline void
z80x::neg8() {
    REG[F] = sub8_flags(0, REG[A]);
    REG[A] = ubyte(0) - REG[A];
}

//------------------------------------------------------------------------------
inline void
z80x::sbc8(ubyte sub) {
    if (REG[F] & CF) {
        // don't waste flag table space for rarely used instructions
        int r = int(REG[A]) - int(sub) - 1;
        ubyte f = NF | YAKC_SZ(r);
        if (r < 0) f |= CF;
        if ((r & 0xF) >= (REG[A] & 0xF)) f |= HF;
        if (((REG[A]&0x80) != (sub&0x80)) && ((r&0x80) != (REG[A]&0x80))) f |= VF;
        REG[F] = f;
        REG[A] = ubyte(r);
    }
    else {
        sub8(sub);
    }
}

//------------------------------------------------------------------------------
inline void
z80x::and8(ubyte val) {
    REG[A] &= val;
    REG[F] = szp[REG[A]]|HF;
}

//------------------------------------------------------------------------------
inline void
z80x::or8(ubyte val) {
    REG[A] |= val;
    REG[F] = szp[REG[A]];
}

//------------------------------------------------------------------------------
inline void
z80x::xor8(ubyte val) {
    REG[A] ^= val;
    REG[F] = szp[REG[A]];
}

//------------------------------------------------------------------------------
inline ubyte
z80x::inc8(ubyte val) {
    ubyte r = val + 1;
    ubyte f = YAKC_SZ(r);
    if ((r & 0xF) == 0) f |= HF;
    if (r == 0x80) f |= VF;
    REG[F] = f | (REG[F] & CF);
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::dec8(ubyte val) {
    ubyte r = val - 1;
    ubyte f = NF | YAKC_SZ(r);
    if ((r & 0xF) == 0xF) f |= HF;
    if (r == 0x7F) f |= VF;
    REG[F] = f | (REG[F] & CF);
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
        REG[F] = f | szp[r];
    }
    else {
        REG[F] = f | (REG[F] & (SF|ZF|PF));
    }
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::rrc8(ubyte val, bool flags_szp) {
    ubyte r = val>>1|val<<7;
    ubyte f = val & CF;
    if (flags_szp) {
        REG[F] = f | szp[r];
    }
    else {
        REG[F] = f | (REG[F] & (SF|ZF|PF));
    }
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::rl8(ubyte val, bool flags_szp) {
    ubyte r = val<<1 | ((REG[F] & CF) ? 0x01:0x00);
    ubyte f = val & 0x80 ? CF : 0;
    if (flags_szp) {
        REG[F] = f | szp[r];
    }
    else {
        REG[F] = f | (REG[F] & (SF|ZF|PF));
    }
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::rr8(ubyte val, bool flags_szp) {
    ubyte r = val>>1 | ((REG[F] & CF) ? 0x80:0x00);
    ubyte f = val & CF;
    if (flags_szp) {
        REG[F] = f | szp[r];
    }
    else {
        REG[F] = f | (REG[F] & (SF|ZF|PF));
    }
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::sla8(ubyte val) {
    ubyte r = val<<1;
    REG[F] = (val & 0x80 ? CF : 0) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::sll8(ubyte val) {
    // undocument! sll8 is identical with sla8 but inserts a 1 into the LSB
    ubyte r = (val<<1) | 1;
    REG[F] = (val & 0x80 ? CF : 0) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::sra8(ubyte val) {
    ubyte r = (val>>1) | (val & 0x80);
    REG[F] = (val & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::srl8(ubyte val) {
    ubyte r = val>>1;
    REG[F] = (val & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline void
z80x::rld() {
    ubyte x = mem.r8(rr16(HL));
    ubyte tmp = REG[A] & 0xF;               // store A low nibble
    REG[A] = (REG[A] & 0xF0) | (x>>4);      // move (HL) high nibble into A low nibble
    x = (x<<4) | tmp;   // move (HL) low to high nibble, move A low nibble to (HL) low nibble
    mem.w8(rr16(HL), x);
    REG[F] = szp[REG[A]] | (REG[F] & CF);
}

//------------------------------------------------------------------------------
inline void
z80x::rrd() {
    ubyte x = mem.r8(rr16(HL));
    ubyte tmp = REG[A] & 0xF;                   // store A low nibble
    REG[A] = (REG[A] & 0xF0) | (x & 0x0F);      // move (HL) low nibble to A low nibble
    x = (x >> 4) | (tmp << 4);  // move A low nibble to (HL) high nibble, and (HL) high nibble to (HL) low nibble
    mem.w8(rr16(HL), x);
    REG[F] = szp[REG[A]] | (REG[F] & CF);
}

//------------------------------------------------------------------------------
inline void
z80x::daa() {
    // from MAME and http://www.z80.info/zip/z80-documented.pdf
    ubyte val = REG[A];
    if (REG[F] & NF) {
        if (((REG[A] & 0xF) > 0x9) || (REG[F] & HF)) {
            val -= 0x06;
        }
        if ((REG[A] > 0x99) || (REG[F] & CF)) {
            val -= 0x60;
        }
    }
    else {
        if (((REG[A] & 0xF) > 0x9) || (REG[F] & HF)) {
            val += 0x06;
        }
        if ((REG[A] > 0x99) || (REG[F] & CF)) {
            val += 0x60;
        }
    }
    REG[F] &= CF|NF;
    REG[F] |= (REG[A] > 0x99) ? CF:0;
    REG[F] |= (REG[A]^val) & HF;
    REG[F] |= szp[val];
    REG[A] = val;
}

//------------------------------------------------------------------------------
inline void
z80x::bit(ubyte val, ubyte mask) {
    ubyte r = val & mask;
    ubyte f = HF | (r ? (r & SF) : (ZF|PF));
    f |= (val & (YF|XF));
    REG[F] = f | (REG[F] & CF);
}

//------------------------------------------------------------------------------
inline bool
z80x::cc(ubyte y) const {
    switch (y) {
        case 0: return !(REG[F] & ZF);  // NZ
        case 1: return REG[F] & ZF;     // Z
        case 2: return !(REG[F] & CF);  // NC
        case 3: return REG[F] & CF;     // C
        case 4: return !(REG[F] & PF);  // PO
        case 5: return REG[F] & PF;     // PE
        case 6: return !(REG[F] & SF);  // P
        default: return REG[F] & SF;    // M
    }
}

//------------------------------------------------------------------------------
inline uword
z80x::add16(uword acc, uword val) {
    unsigned int res = acc + val;
    // flag computation taken from MAME
    REG[F] = (REG[F] & (SF|ZF|VF)) |
        (((acc^res^val)>>8)&HF)|
        ((res>>16) & CF) | ((res >> 8) & (YF|XF));
    return (uword)res;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::fetch_op() {
    R = (R + 1) & 0x7F;
    return mem.r8(PC++);
}

//------------------------------------------------------------------------------
inline int
z80x::d(bool load_d) {
    if (load_d) {
        return mem.rs8(PC++);
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
inline uint32_t
z80x::do_cb(ubyte op, int hl, bool ext, int off) {
    const ubyte x = op>>7;
    const ubyte y = (op>>3) & 7;
    const ubyte z = op & 7;
    uint32_t ext_cycles = ext ? 4 : 0;
    if (x == 0) {
        // roll and shift ops
        ubyte val;
        uword addr;
        if (z == 6) {
            addr = rr16(hl)+off;
            val = mem.r8(addr);
        }
        else {
            val = REG[z];
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
            REG[z] = val;
            return 8;
        }
    }
    else if (x == 1) {
        // BIT y,r[z]
        if (z == 6) {
            const uword addr = rr16(hl)+off;
            bit(mem.r8(addr), 1<<y);
            return 12 + ext_cycles;
        }
        else {
            bit(REG[z], 1<<y);
            return 8;
        }
    }
    else if (x == 2) {
        // RES y,r[z]
        if (z == 6) {
            const uword addr = rr16(hl)+off;
            mem.w8(addr, mem.r8(addr) & ~(1<<y));
            return 15 + ext_cycles;
        }
        else {
            REG[z] &= ~(1<<y);
            return 8;
        }
    }
    else if (x == 3) {
        // SET y,r[z]
        if (z == 6) {
            const uword addr = rr16(hl)+off;
            mem.w8(addr, mem.r8(addr) | (1<<y));
            return 15 + ext_cycles;
        }
        else {
            REG[z] |= (1<<y);
            return 8;
        }
    }
    YAKC_ASSERT(false);
}

//------------------------------------------------------------------------------
inline uint32_t
z80x::do_op(ubyte op, int hl, bool ext) {
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
                const uword addr = rr16(hl)+d(ext);
                mem.w8(addr, REG[z]);
                return 7 + ext_cycles;
            }
        }
        else if (z == 6) {
            // LD r,(HL); LD r,(IX+d); LD r,(IY+d)
            const uword addr = rr16(hl)+d(ext);
            REG[y] = mem.r8(addr);
            return 7 + ext_cycles;
        }
        else {
            // LD r,s
            REG[y] = REG[z];
            return 4;
        }
    }
    else if (x == 2) {
        // 8-bit ALU instruction with register or (HL)
        if (z == 6) {
            const uword addr = rr16(hl)+d(ext);
            alu8(y, mem.r8(addr));
            return 7 + ext_cycles;
        }
        else {
            alu8(y, REG[z]);
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
                        for (int i = 6; i < 8; i++) {
                            ubyte tmp = REG[i];
                            REG[i] = REG_[i];
                            REG_[i] = tmp;
                        }
                        return 4;
                    //--- DJNZ d
                    case 2:
                        if (--REG[B] > 0) {
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
                    //--- LD rp[p],nn (LD BC,nn ..., LD SP,nn)
                    if (p == 3) {
                        SP = mem.r16(PC);
                    }
                    else if (p == 2) {
                        // this can be LD HL,nn; LD IX,nn; LD IY,nn
                        rw16(hl, mem.r16(PC));
                    }
                    else {
                        rw16(p, mem.r16(PC));
                    }
                    PC += 2;
                    return 10;
                }
                else {
                    // ADD HL,rr, ADD IX,rr, ADD IY,rr
                    uword val = (p==3)?SP:((p==2)?rr16(hl):rr16(p));
                    rw16(HL, add16(rr16(hl), val));
                    return 11;
                }
                break;

            //--- indirect loads
            case 2:
                switch (y) {
                    case 0:
                        mem.w8(rr16(BC), REG[A]);       // LD (BC),A
                        return 7;
                    case 1:
                        REG[A] = mem.r8(rr16(BC));      // LD A,(BC)
                        return 7;
                    case 2:
                        mem.w8(rr16(DE), REG[A]);       // LD (DE),A
                        return 7;
                    case 3:
                        REG[A] = mem.r8(rr16(DE));      // LD A,(DE)
                        return 7;
                    case 4:
                        mem.w16(mem.r16(PC), rr16(hl)); // LD (nn),HL
                        PC += 2;
                        return 16;
                    case 5:
                        rw16(hl, mem.r16(mem.r16(PC))); // LD HL,(nn)
                        PC += 2;
                        return 16;
                    case 6:
                        mem.w8(mem.r16(PC), REG[A]);    // LD (nn),A
                        PC += 2;
                        return 13;
                    case 7:
                        REG[A] = mem.r8(mem.r16(PC));   // LD A,(nn)
                        PC += 2;
                        return 13;
                }
                break;

            //--- DEC/INC rp[p] (16-bit)
            case 3:
                if (q == 0) {
                    //--- INC rp[p]
                    if (p == 3) {
                        SP++;
                    }
                    else if (p == 2) {
                        rw16(hl, rr16(hl)+1);
                    }
                    else {
                        rw16(p, rr16(p)+1);
                    }
                }
                else {
                    //--- DEC rp[p]
                    if (p == 3) {
                        SP--;
                    }
                    else if (p == 2) {
                        rw16(hl, rr16(hl)-1);
                    }
                    else {
                        rw16(p, rr16(p)-1);
                    }
                }
                return 6;

            //--- INC r[y] (8-bit)
            case 4:
                if (y == 6) {
                    const uword addr = rr16(hl)+d(ext);
                    mem.w8(addr, inc8(mem.r8(addr)));
                    return 11 + ext_cycles;
                }
                else {
                    REG[y] = inc8(REG[y]);
                    return 4;
                }
                break;
                
            //--- DEC r[y] (8-bit)
            case 5:
                if (y == 6) {
                    const uword addr = rr16(hl)+d(ext);
                    mem.w8(addr, dec8(mem.r8(addr)));
                    return 11 + ext_cycles;
                }
                else {
                    REG[y] = dec8(REG[y]);
                    return 4;
                }
                break;

            //--- LD r[y],n
            case 6:
                if (y == 6) {
                    const uword addr = rr16(hl)+d(ext);
                    mem.w8(addr, mem.r8(PC++));
                    return ext ? 15 : 10;
                }
                else {
                    REG[y] = mem.r8(PC++);
                    return 7;
                }
                break;

            //--- assorted ops on accumulator and flags
            case 7:
                switch (y) {
                    case 0:
                        REG[A] = rlc8(REG[A], false);   // RLCA
                        return 4;
                    case 1:
                        REG[A] = rrc8(REG[A], false);   // RRCA
                        return 4;
                    case 2:
                        REG[A] = rl8(REG[A], false);    // RLA
                        return 4;
                    case 3:
                        REG[A] = rr8(REG[A], false);    // RRA
                        return 4;
                    case 4:
                        daa();                          // DAA
                        return 4;
                    case 5:
                        REG[A] ^= 0xFF;                 // CPL
                        REG[F] = (REG[F]&(SF|ZF|PF|CF)) | HF | NF | (REG[A]&(YF|XF));
                        return 4;
                    case 6:
                        REG[F] = (REG[F]&(SF|ZF|YF|XF|PF))|CF|(REG[A]&(YF|XF)); // SCF
                        return 4;
                    case 7:
                        REG[F] = ((REG[F]&(SF|ZF|YF|XF|PF|CF))|((REG[F]&CF)<<4)|(REG[A]&(YF|XF)))^CF;   // CCF
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
                    // POP rp2[p] (BC=0, DE=1, HL=2, AF=3)
                    if (p == 2) {
                        rw16(hl, mem.r16(SP));
                    }
                    else {
                        rw16(p, mem.r16(SP));
                    }
                    SP += 2;
                    return 10;
                }
                else {
                    if (p == 0) {
                        //--- RET
                        PC = mem.r16(SP);
                        SP += 2;
                        return 10;
                    }
                    else if (p == 1) {
                        //--- EXX (swap BC,DE,HL)
                        for (int i = 0; i < 6; i++) {
                            ubyte tmp = REG[i];
                            REG[i] = REG_[i];
                            REG_[i] = tmp;
                        }
                        return 4;
                    }
                    else if (p == 2) {
                        //--- JP HL; JP IX; JP IY
                        PC = rr16(hl);
                        return 4;
                    }
                    else if (p == 3) {
                        //--- LD SP,HL; LD SP,IX; LD SP,IY
                        SP = rr16(hl);
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
                    case 0: // JP nn
                        PC = mem.r16(PC);
                        return 10;
                    case 1: // CB prefix
                        {
                            const int off = d(ext);
                            const ubyte op = fetch_op();
                            return do_cb(op, hl, ext, off);
                        }
                    case 2: // OUT (n),A
                        out((REG[A]<<8)|mem.r8(PC++), REG[A]);
                        return 11;
                    case 3: // IN A,(n)
                        REG[A] = in((REG[A]<<8)|(mem.r8(PC++)));
                        return 11;
                    case 4: // EX (SP),HL; EX (SP),IX; EX (SP),IY
                        {
                            uword tmp = mem.r16(SP);
                            mem.w16(SP, rr16(hl));
                            rw16(hl, tmp);
                        }
                        return 19;
                    case 5: // EX DE,HL
                        {
                            uword tmp = rr16(DE);
                            rw16(DE, rr16(HL));
                            rw16(HL, tmp);
                        }
                        return 4;
                    case 6: // DI
                        di();
                        return 4;
                    case 7: // EI
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
                    if (p == 2) {
                        mem.w16(SP, rr16(hl));
                    }
                    else {
                        mem.w16(SP, rr16(p));
                    }
                    return 11;
                }
                else {
                    if (p == 0) {
                        // CALL nn
                        SP -= 2;
                        mem.w16(SP, PC+2);
                        PC = mem.r16(PC);
                        return 17;
                    }
                    else if (p == 1) {
                        // DD prefix
                        if (!ext) {
                            return do_op(fetch_op(), IX, true) + 4;
                        }
                        else {
                            return invalid_opcode(2);
                        }
                    }
                    else if (p == 2) {
                        // FIXME: ED prefix!
                        YAKC_ASSERT(false);
                    }
                    else if (p == 3) {
                        // FD prefix
                        if (!ext) {
                            return do_op(fetch_op(), IY, true) + 4;
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
    const ubyte op = fetch_op();
    return do_op(op, HL, false);
}

} // namespace yakc

