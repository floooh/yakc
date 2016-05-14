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

        BC = 0,
        DE = 1,
        HL = 2,
        AF = 3,

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
    ubyte REG[8];
    /// shadow register set
    ubyte REG_[8];
    /// the IX index register
    union {
        struct { ubyte IXL, IXH; };
        uword IX;
    };
    /// the IY index register
    union {
        struct { ubyte IYL, IYH; };
        uword IY;
    };
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

    /// halt instruction
    void halt();
    /// rst instruction (vec is 0x00, 0x08, 0x10, ...)
    void rst(ubyte vec);

    /// fetch an opcode byte and increment R register
    ubyte fetch_op();
    /// execute a single instruction, return number of cycles
    uint32_t step();
};

#define YAKC_SZ(val) ((val&0xFF)?(val&SF):ZF)

//------------------------------------------------------------------------------
inline void
z80x::halt() {
    HALT = true;
    PC--;
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
        // don't waste flag table space for rarely used instructions
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
inline void
z80x::rst(ubyte vec) {
    SP -= 2;
    mem.w16(SP, PC);
    PC = (uword) vec;
}

//------------------------------------------------------------------------------
inline ubyte
z80x::fetch_op() {
    R = (R + 1) & 0x7F;
    return mem.r8(PC++);
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
inline bool
z80x::cc(ubyte y) {
    switch (y) {
        case 0: return !(REG[F] & ZF);  // NZ
        case 1: return REG[F] & ZF;     // Z
        case 2: return !(REG[F] & CF);  // NC
        case 3: return REG[F] & CF;     // C
        case 4: return !(REG[F] & PF);  // PO
        case 5: return REG[F] & PF;     // PE
        case 6: return !(REG[F] & SF);  // P
        case 7: return REG[F] & SF;     // M
    }
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
    const ubyte x = op>>6;
    const ubyte y = (op>>3) & 7;
    const ubyte z = op & 7;
    if (x == 1) {
        // 8-bit load, or special case HALT for LD (HL),(HL)
        if (y == 6) {
            if (z == 6) {
                halt();
                return 4;
            }
            else {
                // LD (HL),r
                mem.w8(rr16(HL), r[z]);
                return 7;
            }
        }
        else if (z == 6) {
            // LD r,(HL)
            r[y] = mem.r8(rr16(HL));
            return 7;
        }
        else {
            // LD r,s
            r[y] = r[z];
            return 4;
        }
    }
    else if (x == 2) {
        // 8-bit ALU instruction with register or (HL)
        ubyte val;
        uint32_t cycles;
        if (z == 6) {
            alu8(y, mem.r8(rr16(HL)));
            return 7;
        }
        else {
            alu8(y, r[z]);
            return 4;
        }
    }
    else if (x == 0) {
        switch (z) {
            case 0: break;
            case 1: break;
            case 2: break;
            case 3: break;
            case 4: break;
            case 5: break;
            case 6: break;
            case 7: break;
        }
        o_error("FIXME!");
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
                    rw16(p, mem.r16(SP));
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
                            ubyte tmp = r[i];
                            r[i] = r_[i];
                            r_[i] = r[i];
                        }
                        return 4;
                    }
                    else if (p == 2) {
                        //--- JP HL
                        PC = rr16(HL);
                        return 4;
                    }
                    else if (p == 3) {
                        //--- LD SP,HL
                        SP = rr16(HL);
                        return 6;
                    }
                }
                break;

            //--- JP cc
            case 2: break;
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
                    case 1: // FIXME: CB prefix!
                        break;
                    case 2: // OUT (n),A
                        out((REG[A]<<8)|mem.r8(PC++), REG[A]);
                        return 11;
                    case 3: // IN A,(n)
                        REG[A] = in((REG[A]<<8)|(mem.r8(PC++)));
                        return 11;
                    case 4: // EX (SP),HL
                        {
                            uword tmp = mem.r16(SP);
                            mem.w16(SP, rr16(HL));
                            rw16(HL, tmp);
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
            case 4: break;
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

            //--- PUSH and various ops
            case 5:
                if (q == 0) {
                    // PUSH rp2[p] (BC=0, DE=1, HL=2, AF=3)
                    SP -= 2;
                    mem.w16(rr16(p));
                    return 11;
                }
                else {
                    if (p == 0) {
                        // CALL nn
                        SP -= 2;
                        mem.w16(SP, PC+2);
                        PC = mem.r16(PC);
                    }
                    else if (p == 1) {
                        // FIXME: DD prefix!
                        o_error("FIXME: DD prefix");
                    }
                    else if (p == 2) {
                        // FIXME: ED prefix!
                        o_error("FIXME: ED prefix");
                    }
                    else if (p == 3) {
                        // FIXME: FD prefix!
                        o_error("FIXME: FD prefix");
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
    o_error("FIXME!\n");
}

} // namespace yakc

