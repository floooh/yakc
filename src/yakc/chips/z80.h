#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z80
    @brief the Z80 (or rather U880) cpu
*/
#include "yakc/core/core.h"
#include "yakc/core/memory.h"
#include "yakc/chips/z80int.h"

namespace YAKC {

class system_bus;
class z80 {
public:
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
    uword BC_;
    uword DE_;
    uword HL_;
    uword AF_;
    uword WZ_;
    uword SP;
    uword PC;
    ubyte I;
    ubyte R;
    ubyte IM;

    bool HALT;
    bool IFF1, IFF2;
    bool INV;   // invalid instruction hit

    /// flag lookup table for SZP flag combinations
    ubyte szp[256];

    /// memory map
    memory mem;

    /// highest priority interrupt controller in daisy chain
    z80int* irq_device;
    /// INT line is currently active
    bool int_active;
    /// delayed-interrupt-enable flag set bei ei()
    bool int_enable;
    /// break on invalid opcode?
    bool break_on_invalid_opcode;

    /// constructor
    z80();

    /// one-time init
    void init();
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

    /// trigger interrupt request line on or off
    void irq(bool b);
    /// handle interrupt request
    uint32_t handle_irq(system_bus* bus);
    /// implement the RETI instruction
    void reti();
    /// implement the EI instruction
    void ei();
    /// implement the DI instruction
    void di();

    /// call in-handler, return result
    ubyte in(system_bus* bus, uword port);
    /// call out-handler
    void out(system_bus* bus, uword port, ubyte val);

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
    void ini(system_bus* bus);
    /// implement the INIR instruction, return number of T-states
    int inir(system_bus* bus);
    /// implement the IND instruction
    void ind(system_bus* bus);
    /// implement the INDR instruction, return number of T-states
    int indr(system_bus* bus);
    /// return flags for outi/outd instruction
    ubyte outi_outd_flags(ubyte io_val);
    /// implement the OUTI instruction
    void outi(system_bus* bus);
    /// implement the OTIR instructor, return number of T-states
    int otir(system_bus* bus);
    /// implment the OUTD instruction
    void outd(system_bus* bus);
    /// implement the OTDR instruction, return number of T-states
    int otdr(system_bus* bus);
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
    uint32_t step(system_bus* bus);
    /// top-level opcode decoder (generated)
    uint32_t do_op(system_bus* bus);
};

#define YAKC_SZ(val) ((val&0xFF)?(val&SF):ZF)
#define YAKC_SZYXCH(acc,val,res) (YAKC_SZ(res)|(res&(YF|XF))|((res>>8)&CF)|((acc^val^res)&HF))

//------------------------------------------------------------------------------
inline void
z80::swap16(uword& r0, uword& r1) {
    uword tmp = r0;
    r0 = r1;
    r1 = tmp;
}

//------------------------------------------------------------------------------
#define YAKC_ADD_FLAGS(acc,val,res) (YAKC_SZYXCH(acc,val,res)|((((val^acc^0x80)&(val^res))>>5)&VF))
inline void
z80::add8(ubyte add) {
    int r = A + add;
    F = YAKC_ADD_FLAGS(A,add,r);
    A = ubyte(r);
}

//------------------------------------------------------------------------------
inline void
z80::adc8(ubyte add) {
    int r = A + add + (F&CF);
    F = YAKC_ADD_FLAGS(A,add,r);
    A = ubyte(r);
}

//------------------------------------------------------------------------------
#define YAKC_SUB_FLAGS(acc,val,res) (NF|YAKC_SZYXCH(acc,val,res)|((((val^acc)&(res^acc))>>5)&VF))
inline void
z80::sub8(ubyte sub) {
    int r = int(A) - int(sub);
    F = YAKC_SUB_FLAGS(A,sub,r);
    A = ubyte(r);
}

//------------------------------------------------------------------------------
#define YAKC_CP_FLAGS(acc,val,res) (NF|(YAKC_SZ(res)|(val&(YF|XF))|((res>>8)&CF)|((acc^val^res)&HF))|((((val^acc)&(res^acc))>>5)&VF))
inline void
z80::cp8(ubyte sub) {
    // NOTE: XF|YF are set from sub, not from result!
    int r = int(A) - int(sub);
    F = YAKC_CP_FLAGS(A,sub,r);
}

//------------------------------------------------------------------------------
inline void
z80::neg8() {
    uint8_t sub = A;
    A = 0;
    sub8(sub);
}

//------------------------------------------------------------------------------
inline void
z80::sbc8(ubyte sub) {
    int r = int(A) - int(sub) - (F&CF);
    F = YAKC_SUB_FLAGS(A,sub,r);
    A = ubyte(r);
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
    ubyte f = YAKC_SZ(r)|(r&(XF|YF))|((r^val)&HF);
    if (r == 0x80) f |= VF;
    F = f | (F&CF);
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::dec8(ubyte val) {
    ubyte r = val - 1;
    ubyte f = NF|YAKC_SZ(r)|(r&(XF|YF))|((r^val)&HF);
    if (r == 0x7F) f |= VF;
    F = f | (F & CF);
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::rlc8(ubyte val) {
    ubyte r = val<<1|val>>7;
    F = szp[r] | (val>>7 & CF);
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::rlca8() {
    ubyte r = A<<1|A>>7;
    F = (A>>7 & CF) | (F & (SF|ZF|PF)) | (r&(XF|YF));
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
    F = (val>>7 & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::rla8() {
    ubyte r = A<<1 | (F & CF);
    F = (A>>7 & CF) | (F & (SF|ZF|PF)) | (r&(YF|XF));
    A = r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::rr8(ubyte val) {
    ubyte r = val>>1 | ((F & CF)<<7);
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
    F = (val>>7 & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline ubyte
z80::sll8(ubyte val) {
    // undocument! sll8 is identical with sla8 but inserts a 1 into the LSB
    ubyte r = (val<<1) | 1;
    F = (val>>7 & CF) | szp[r];
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
inline ubyte
z80::fetch_op() {
    R = (R&0x80) | ((R+1)&0x7F);
    return mem.r8(PC++);
}

//------------------------------------------------------------------------------
inline uint32_t
z80::step(system_bus* bus) {
    INV = false;
    if (int_enable) {
        IFF1 = IFF2 = true;
        int_enable = false;
    }
    return do_op(bus);
}

} // namespace YAKC
