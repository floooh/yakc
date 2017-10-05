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
        struct { uint8_t C, B; };
        uint16_t BC;
    };
    union {
        struct { uint8_t E, D; };
        uint16_t DE;
    };
    union {
        struct { uint8_t L, H; };
        uint16_t HL;
    };
    union {
        struct { uint8_t F, A; };
        uint16_t AF;
    };
    union {
        struct { uint8_t IXL, IXH; };
        uint16_t IX;
    };
    union {
        struct { uint8_t IYL, IYH; };
        uint16_t IY;
    };
    union {
        struct { uint8_t Z, W; };
        uint16_t WZ;
    };
    uint16_t BC_;
    uint16_t DE_;
    uint16_t HL_;
    uint16_t AF_;
    uint16_t WZ_;
    uint16_t SP;
    uint16_t PC;
    uint8_t I;
    uint8_t R;
    uint8_t IM;

    bool HALT;
    bool IFF1, IFF2;
    bool INV;   // invalid instruction hit

    /// flag lookup table for SZP flag combinations
    uint8_t szp[256];

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
    /// cycle-count tables (each with 256 entries)
    const uint32_t* cc_op;
    const uint32_t* cc_cb;
    const uint32_t* cc_ed;
    const uint32_t* cc_xy;
    const uint32_t* cc_xycb;
    const uint32_t* cc_ex;    // extra cycles conditional jumps and rst

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
    bool test_flags(uint8_t expected) const;
    /// called when invalid opcode has been hit
    uint32_t invalid_opcode(uint16_t opsize);
    /// helper method to swap 2 16-bit registers
    static void swap16(uint16_t& r0, uint16_t& r1);

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
    uint8_t in(system_bus* bus, uint16_t port);
    /// call out-handler
    void out(system_bus* bus, uint16_t port, uint8_t val);

    /// get flags for the LD A,I and LD A,R instructions
    static uint8_t sziff2(uint8_t val, bool iff2);

    /// halt instruction
    void halt();
    /// rst instruction (vec is 0x00, 0x08, 0x10, ...)
    void rst(uint8_t vec);
    /// perform an 8-bit add and update flags
    void add8(uint8_t add);
    /// perform an 8-bit adc and update flags
    void adc8(uint8_t add);
    /// perform an 8-bit sub and update flags
    void sub8(uint8_t sub);
    /// perform an 8-bit sbc and update flags
    void sbc8(uint8_t sub);
    /// perform 8-bit compare (identical with sub8 but throw result away)
    void cp8(uint8_t sub);
    /// perform a neg and update flags
    void neg8();
    /// perform an 8-bit and and update flags
    void and8(uint8_t val);
    /// perform an 8-bit or and update flags
    void or8(uint8_t val);
    /// perform an 8-bit xor and update flags
    void xor8(uint8_t val);
    /// perform an 8-bit inc and set flags
    uint8_t inc8(uint8_t val);
    /// perform an 8-bit dec and set flags
    uint8_t dec8(uint8_t val);
    /// perform an 16-bit add, update flags and return result
    uint16_t add16(uint16_t acc, uint16_t val);
    /// perform an 16-bit adc, update flags and return result
    uint16_t adc16(uint16_t acc, uint16_t val);
    /// perform an 16-bit sbc, update flags and return result
    uint16_t sbc16(uint16_t acc, uint16_t val);
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
    uint8_t ini_ind_flags(uint8_t io_val, int c_add);
    /// implement the INI instruction
    void ini(system_bus* bus);
    /// implement the INIR instruction, return number of T-states
    int inir(system_bus* bus);
    /// implement the IND instruction
    void ind(system_bus* bus);
    /// implement the INDR instruction, return number of T-states
    int indr(system_bus* bus);
    /// return flags for outi/outd instruction
    uint8_t outi_outd_flags(uint8_t io_val);
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
    uint8_t rlc8(uint8_t val);
    /// rotate A left, copy sign bit into CF
    void rlca8();
    /// rotate right, copy bit 0 into CF
    uint8_t rrc8(uint8_t val);
    /// rotate A right, copy bit 0 into CF
    void rrca8();
    /// rotate left through carry bit
    uint8_t rl8(uint8_t val);
    /// rotate A left through carry bit
    void rla8();
    /// rotate right through carry bit
    uint8_t rr8(uint8_t val);
    /// rotate A right through carry bit
    void rra8();
    /// shift left into carry bit and update flags
    uint8_t sla8(uint8_t val);
    /// undocumented: shift left into carry bit, update flags
    uint8_t sll8(uint8_t val);
    /// shift right into carry bit, preserve sign, update flags
    uint8_t sra8(uint8_t val);
    /// shift right into carry bit, update flags
    uint8_t srl8(uint8_t val);
    /// implements the RLD instruction
    void rld();
    /// implements the RRD instruction
    void rrd();
    /// implements the BIT test instruction, updates flags
    void bit(uint8_t val, uint8_t mask);
    /// same as BIT, but take undocumented YF|XF flags from undocumented WZ register
    void ibit(uint8_t val, uint8_t mask);

    /// fetch an opcode byte and increment R register
    uint8_t fetch_op();
    /// execute a single instruction, return number of cycles
    uint32_t step(system_bus* bus);
    /// top-level opcode decoder (generated)
    uint32_t do_op(system_bus* bus);
};

#define YAKC_SZ(val) ((val&0xFF)?(val&SF):ZF)
#define YAKC_SZYXCH(acc,val,res) (YAKC_SZ(res)|(res&(YF|XF))|((res>>8)&CF)|((acc^val^res)&HF))

//------------------------------------------------------------------------------
inline void
z80::swap16(uint16_t& r0, uint16_t& r1) {
    uint16_t tmp = r0;
    r0 = r1;
    r1 = tmp;
}

//------------------------------------------------------------------------------
#define YAKC_ADD_FLAGS(acc,val,res) (YAKC_SZYXCH(acc,val,res)|((((val^acc^0x80)&(val^res))>>5)&VF))
inline void
z80::add8(uint8_t add) {
    int r = A + add;
    F = YAKC_ADD_FLAGS(A,add,r);
    A = uint8_t(r);
}

//------------------------------------------------------------------------------
inline void
z80::adc8(uint8_t add) {
    int r = A + add + (F&CF);
    F = YAKC_ADD_FLAGS(A,add,r);
    A = uint8_t(r);
}

//------------------------------------------------------------------------------
#define YAKC_SUB_FLAGS(acc,val,res) (NF|YAKC_SZYXCH(acc,val,res)|((((val^acc)&(res^acc))>>5)&VF))
inline void
z80::sub8(uint8_t sub) {
    int r = int(A) - int(sub);
    F = YAKC_SUB_FLAGS(A,sub,r);
    A = uint8_t(r);
}

//------------------------------------------------------------------------------
#define YAKC_CP_FLAGS(acc,val,res) (NF|(YAKC_SZ(res)|(val&(YF|XF))|((res>>8)&CF)|((acc^val^res)&HF))|((((val^acc)&(res^acc))>>5)&VF))
inline void
z80::cp8(uint8_t sub) {
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
z80::sbc8(uint8_t sub) {
    int r = int(A) - int(sub) - (F&CF);
    F = YAKC_SUB_FLAGS(A,sub,r);
    A = uint8_t(r);
}

//------------------------------------------------------------------------------
inline void
z80::and8(uint8_t val) {
    A &= val;
    F = szp[A]|HF;
}

//------------------------------------------------------------------------------
inline void
z80::or8(uint8_t val) {
    A |= val;
    F = szp[A];
}

//------------------------------------------------------------------------------
inline void
z80::xor8(uint8_t val) {
    A ^= val;
    F = szp[A];
}

//------------------------------------------------------------------------------
inline uint8_t
z80::inc8(uint8_t val) {
    uint8_t r = val + 1;
    uint8_t f = YAKC_SZ(r)|(r&(XF|YF))|((r^val)&HF);
    if (r == 0x80) f |= VF;
    F = f | (F&CF);
    return r;
}

//------------------------------------------------------------------------------
inline uint8_t
z80::dec8(uint8_t val) {
    uint8_t r = val - 1;
    uint8_t f = NF|YAKC_SZ(r)|(r&(XF|YF))|((r^val)&HF);
    if (r == 0x7F) f |= VF;
    F = f | (F & CF);
    return r;
}

//------------------------------------------------------------------------------
inline uint8_t
z80::rlc8(uint8_t val) {
    uint8_t r = val<<1|val>>7;
    F = szp[r] | (val>>7 & CF);
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::rlca8() {
    uint8_t r = A<<1|A>>7;
    F = (A>>7 & CF) | (F & (SF|ZF|PF)) | (r&(XF|YF));
    A = r;
}

//------------------------------------------------------------------------------
inline uint8_t
z80::rrc8(uint8_t val) {
    uint8_t r = val>>1|val<<7;
    F = szp[r] | (val & CF);
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::rrca8() {
    uint8_t r = A>>1|A<<7;
    F = (A & CF) | (F & (SF|ZF|PF)) | (r&(YF|XF));
    A = r;
}

//------------------------------------------------------------------------------
inline uint8_t
z80::rl8(uint8_t val) {
    uint8_t r = val<<1 | (F & CF);
    F = (val>>7 & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::rla8() {
    uint8_t r = A<<1 | (F & CF);
    F = (A>>7 & CF) | (F & (SF|ZF|PF)) | (r&(YF|XF));
    A = r;
}

//------------------------------------------------------------------------------
inline uint8_t
z80::rr8(uint8_t val) {
    uint8_t r = val>>1 | ((F & CF)<<7);
    F = (val & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::rra8() {
    uint8_t r = A>>1 | ((F & CF)<<7);
    F = (A & CF) | (F & (SF|ZF|PF)) | (r&(YF|XF));
    A = r;
}

//------------------------------------------------------------------------------
inline uint8_t
z80::sla8(uint8_t val) {
    uint8_t r = val<<1;
    F = (val>>7 & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline uint8_t
z80::sll8(uint8_t val) {
    // undocument! sll8 is identical with sla8 but inserts a 1 into the LSB
    uint8_t r = (val<<1) | 1;
    F = (val>>7 & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline uint8_t
z80::sra8(uint8_t val) {
    uint8_t r = (val>>1) | (val & 0x80);
    F = (val & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline uint8_t
z80::srl8(uint8_t val) {
    uint8_t r = val>>1;
    F = (val & CF) | szp[r];
    return r;
}

//------------------------------------------------------------------------------
inline void
z80::bit(uint8_t val, uint8_t mask) {
    uint8_t r = val & mask;
    uint8_t f = HF | (r ? (r & SF) : (ZF|PF));
    f |= (val & (YF|XF));
    F = f | (F & CF);
}

//------------------------------------------------------------------------------
inline void
z80::ibit(uint8_t val, uint8_t mask) {
    // this is the version for the BIT instruction for (HL), (IX+d), (IY+d),
    // these set the undocumented YF and XF flags from high byte of HL+1
    // or IX/IY+d
    uint8_t r = val & mask;
    uint8_t f = HF | (r ? (r & SF) : (ZF|PF));
    f |= (W & (YF|XF));
    F = f | (F & CF);
}

//------------------------------------------------------------------------------
inline uint16_t
z80::add16(uint16_t acc, uint16_t val) {
    WZ = acc+1;
    uint32_t res = acc + val;
    // flag computation taken from MAME
    F = (F & (SF|ZF|VF)) |
        (((acc^res^val)>>8)&HF)|
        ((res>>16) & CF) | ((res >> 8) & (YF|XF));
    return (uint16_t)res;
}

//------------------------------------------------------------------------------
inline uint16_t
z80::adc16(uint16_t acc, uint16_t val) {
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
inline uint16_t
z80::sbc16(uint16_t acc, uint16_t val) {
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
inline uint8_t
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
