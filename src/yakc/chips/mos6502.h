#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::mos6502
    @brief MOS Technologies 6502 CPU emulation
*/
#include "yakc/core/core.h"
#include "yakc/core/memory.h"

namespace YAKC {

class system_bus;

namespace mos6502_enums {

// addressing modes
enum {
    A____,      // no addressing mode
    A_IMM,      // #
    A_ZER,      // zp
    A_ZPX,      // zp,X
    A_ZPY,      // zp,Y
    A_ABS,      // abs
    A_ABX,      // abs,X
    A_ABY,      // abs,Y
    A_IDX,      // (zp,X)
    A_IDY,      // (zp),Y
    A_JMP,      // special JMP abs
    A_JSR,      // special JSR abs

    A_INV,      // this is an invalid instruction
};
// memory addressing modes
enum {
    M___,       // no memory access
    M_R_,        // read
    M__W,        // write in current cycle
    M_RW,       // read-modify-write
};

};

class mos6502 {
public:
    // status flags
    enum {
        CF = (1<<0),    // carry
        ZF = (1<<1),    // zero
        IF = (1<<2),    // IRQ disable
        DF = (1<<3),    // decimal mode
        BF = (1<<4),    // BRK command
        XF = (1<<5),    // unused
        VF = (1<<6),    // overflow
        NF = (1<<7),    // negative
    };

    // behaviour flags
    bool bcd_enabled;   // default is true, set to false on CPUs that don't support BCD

    // registers
    uint8_t A,X,Y,S,P;
    uint16_t PC;
    uint8_t OP;

    // pins
    bool RW;            // read: true, write: false
    uint16_t ADDR;      // current address bus value
    uint8_t DATA;       // current data bus value
    uint16_t tmp16;

    int Cycle;              // current instruction cycle
    uint8_t AddrMode;       // currently active addressing mode
    uint8_t MemAccess;      // M_R, M_W, M_RW

    // addressing mode table
    struct op_desc {
        uint8_t addr;       // addressing mode
        uint8_t mem;        // memory access mode
    };
    static op_desc ops[4][8][8];

    // memory map
    memory mem;

    // pointer to system bus object
    system_bus* bus;

    /// constructor
    mos6502();

    /// one-time init
    void init(system_bus* bus);
    /// reset the CPU
    void reset();
    /// called when an invalid opcode has been hit
    uint32_t invalid_opcode();

    /// trigger interrupt request line on/off
    void irq(bool b);
    /// handle an interrupt request
    int handle_irq();

    /// execute next instruction, return cycles
    uint32_t step();

    /// read byte from mem, incr cycle, tick bus
    void read();
    /// write byte to mem, incr cycle, tick bus
    void write();
    /// fetch and decode the next instruction
    void fetch();
    /// determine address and put on address bus
    void addr();
    /// execute instruction
    void exec();

    /// helper funcs
    void do_sbc(uint8_t val);
    void do_adc(uint8_t val);
    void do_cmp(uint8_t val);
    uint8_t do_asl(uint8_t val);
    uint8_t do_rol(uint8_t val);
    uint8_t do_lsr(uint8_t val);
    uint8_t do_ror(uint8_t val);

    // instructions
    void nop();
    void brk();

    void lda();
    void ldx();
    void ldy();
    void sta();
    void stx();
    void sty();
    void tax();
    void tay();
    void txa();
    void tya();
    void txs();
    void tsx();

    void php();
    void plp();
    void pha();
    void pla();

    void se(uint8_t f);
    void cl(uint8_t f);

    void br(uint8_t m, uint8_t v); // branch if (p & m) == v
    void jmp();
    void jmpi();

    void jsr();
    void rts();
    void rti();

    void ora();
    void anda();
    void eor();
    void adc();
    void cmp();
    void cpx();
    void cpy();
    void sbc();
    void dec();
    void dex();
    void dey();
    void inc();
    void inx();
    void iny();
    void asl();
    void asla();
    void lsr();
    void lsra();
    void rol();
    void rola();
    void ror();
    void rora();
    void bit();

    // undocumented instructions
    void u_nop();
    void u_lax();
    void u_sax();
    void u_sbc();
    void u_dcp();
    void u_isb();
    void u_slo();
    void u_rla();
    void u_sre();
    void u_rra();
};

// set N and Z flags on P depending on V, return new P
#define YAKC_MOS6502_NZ(p,v) ((p & ~(NF|ZF)) | ((v & 0xFF) ? (v & NF):ZF))

//------------------------------------------------------------------------------
inline void
mos6502::brk() {
    read();
    //--
    ADDR = 0x0100 | S--; DATA = PC>>8;
    write();
    //--
    ADDR = 0x0100 | S--; DATA = PC;
    write();
    //--
    ADDR = 0x0100 | S--; DATA = P | BF;
    write();
    //--
    ADDR = 0xFFFE;
    read();
    //--
    tmp16 = DATA;
    ADDR = 0xFFFF;
    read();
    PC = (DATA<<8) | (tmp16&0x00FF);
    P = (P | IF) & ~DF;
}

//------------------------------------------------------------------------------
inline void
mos6502::nop() {
    read();
}

//------------------------------------------------------------------------------
inline void
mos6502::u_nop() {
    read();
}

//------------------------------------------------------------------------------
inline void
mos6502::lda() {
    read();
    A = DATA; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::ldx() {
    read();
    X = DATA; P = YAKC_MOS6502_NZ(P,X);
}

//------------------------------------------------------------------------------
inline void
mos6502::ldy() {
    read();
    Y = DATA; P = YAKC_MOS6502_NZ(P,Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::u_lax() {
    read();
    A = X = DATA; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::sta() {
    DATA = A;
    write();
}

//------------------------------------------------------------------------------
inline void
mos6502::stx() {
    DATA = X;
    write();
}

//------------------------------------------------------------------------------
inline void
mos6502::sty() {
    DATA = Y;
    write();
}

//------------------------------------------------------------------------------
inline void
mos6502::u_sax() {
    DATA = A & X;
    write();
}

//------------------------------------------------------------------------------
inline void
mos6502::tax() {
    read();
    X = A; P = YAKC_MOS6502_NZ(P,X);
}

//------------------------------------------------------------------------------
inline void
mos6502::tay() {
    read();
    Y = A; P = YAKC_MOS6502_NZ(P,Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::txa() {
    read();
    A = X; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::tya() {
    read();
    A = Y; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::txs() {
    read();
    S = X;
}

//------------------------------------------------------------------------------
inline void
mos6502::tsx() {
    read();
    X = S; P = YAKC_MOS6502_NZ(P,X);
}

//------------------------------------------------------------------------------
inline void
mos6502::php() {
    read();
    //--
    ADDR = 0x0100|S--; DATA = (P | BF);
    write();
}

//------------------------------------------------------------------------------
inline void
mos6502::plp() {
    read();
    //-- first read junk from current SP
    ADDR = 0x0100|S++;
    read();
    //-- read actual byte
    ADDR = 0x0100|S;
    read();
    P = (DATA & ~BF) | XF;
}

//------------------------------------------------------------------------------
inline void
mos6502::pha() {
    read();
    //--
    ADDR = 0x0100|S--; DATA = A;
    write();
}

//------------------------------------------------------------------------------
inline void
mos6502::pla() {
    read();
    //-- first read junk from current SP
    ADDR = 0x0100|S++;
    read();
    // read actual byte
    ADDR = 0x0100|S;
    read();
    A = DATA; P = YAKC_MOS6502_NZ(P, A);
}

//------------------------------------------------------------------------------
inline void
mos6502::se(uint8_t f) {
    read();
    P |= f;
}

//------------------------------------------------------------------------------
inline void
mos6502::cl(uint8_t f) {
    read();
    P &= ~f;
}

//------------------------------------------------------------------------------
inline void
mos6502::br(uint8_t m, uint8_t v) {
    read();
    // check if the branch is taken, if not return after 2 cycles
    if ((P & m) != v) {
        return;
    }
    //--
    read();
    // branch was taken, compute target address, return after 3
    // cycles if target address is in same 256 bytes page
    tmp16 = PC + int8_t(DATA);
    if ((tmp16 & 0xFF00) == (PC & 0xFF00)) {
        PC = tmp16;
        return;
    }
    read();
    // page boundary was crossed, return after 4 cycles
    PC = tmp16;
}

//------------------------------------------------------------------------------
inline void
mos6502::jmp() {
    read();
    PC = (DATA<<8) | (tmp16 & 0x00FF);
}

//------------------------------------------------------------------------------
inline void
mos6502::jmpi() {
    read();
    // load first byte of target address
    ADDR = (DATA<<8) | (tmp16 & 0x00FF);
    read();
    // load second byte of target address, wrap around in current page
    tmp16 = DATA;
    ADDR = (ADDR & 0xFF00) | ((ADDR + 1) & 0x00FF);
    read();
    // form target address in PC
    PC = (DATA<<8) | (tmp16 & 0x00FF);
}

//------------------------------------------------------------------------------
inline void
mos6502::jsr() {
    read();
    // low-byte of target address loaded, and put SP
    // on addr bus, but the next cycle is a junk read
    tmp16 = DATA; ADDR = 0x0100 | S;
    read();
    // write PC high byte to stack
    DATA = PC >> 8;
    ADDR = 0x0100 | S--;
    write();
    // write PC low byte to stack
    ADDR = 0x0100 | S--;
    DATA = PC;
    write();
    // load the target address high-byte
    ADDR = PC;
    read();
    // and finally jump to target address
    PC = (DATA<<8) | (tmp16 & 0x00FF);
}

//------------------------------------------------------------------------------
inline void
mos6502::rts() {
    read();
    // first put SP on addr bus and do a junk read
    ADDR = 0x0100 | S++;
    read();
    // read return addr low byte
    ADDR = 0x0100 | S++;
    read();
    // keep return addr low byte, and read high byte from stack
    tmp16 = DATA; ADDR = 0x0100 | S;
    read();
    // put return address in PC, this is one byte before next op, do a junk read
    PC = (DATA<<8) | (tmp16 & 0x00FF);
    ADDR = PC++;
    read();
}

//------------------------------------------------------------------------------
inline void
mos6502::rti() {
    read();
    // first put SP on addr bus and do a junk read
    ADDR = 0x0100 | S++;
    read();
    // read processor status flag
    ADDR = 0x0100 | S++;
    read();
    // store P, read return addr low byte
    P = (DATA & ~BF) | XF; ADDR = 0x0100 | S++;
    read();
    // store return-addr low byte, read high byte
    tmp16 = DATA; ADDR = 0x0100 | S;
    read();
    // update PC
    PC = (DATA<<8) | (tmp16 & 0x00FF);
}

//------------------------------------------------------------------------------
inline void
mos6502::ora() {
    read();
    A |= DATA; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::anda() {
    read();
    A &= DATA; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::eor() {
    read();
    A ^= DATA; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::do_adc(uint8_t val) {
    // from MAME
    if (bcd_enabled && (P & DF)) {
        // decimal mode
        uint8_t c  = P & CF ? 1 : 0;
        P &= ~(NF|VF|ZF|CF);
        uint8_t al = (A & 0x0F) + (val & 0x0F) + c;
        if (al > 9) {
            al += 6;
        }
        uint8_t ah = (A >> 4) + (val >> 4) + (al > 0x0F);
        if (0 == (A + val + c)) {
            P |= ZF;
        }
        else if (ah & 0x08) {
            P |= NF;
        }
        if (~(A^val) & (A^(ah<<4)) & 0x80) {
            P |= VF;
        }
        if (ah > 9) {
            ah += 6;
        }
        if (ah > 15) {
            P |= CF;
        }
        A = (ah<<4) | (al & 0x0F);
    }
    else {
        // default mode
        uint16_t sum = A + val + (P & CF ? 1:0);
        P &= ~(VF|CF);
        P = YAKC_MOS6502_NZ(P, uint8_t(sum));
        if (~(A^val) & (A^sum) & 0x80) {
            P |= VF;
        }
        if (sum & 0xFF00) {
            P |= CF;
        }
        A = sum;
    }
}

//------------------------------------------------------------------------------
inline void
mos6502::adc() {
    read();
    do_adc(DATA);
}

//------------------------------------------------------------------------------
inline void
mos6502::do_sbc(uint8_t val) {
    // from MAME
    if (bcd_enabled && (P & DF)) {
        // decimal mode
        uint8_t c = P & CF ? 0 : 1;
        P &= ~(NF|VF|ZF|CF);
        uint16_t diff = A - val - c;
        uint8_t al = (A & 0x0F) - (val & 0x0F) - c;
        if (int8_t(al) < 0) {
            al -= 6;
        }
        uint8_t ah = (A>>4) - (val>>4) - (int8_t(al) < 0);
        if (0 == uint8_t(diff)) {
            P |= ZF;
        }
        else if (diff & 0x80) {
            P |= NF;
        }
        if ((A^val) & (A^diff) & 0x80) {
            P |= VF;
        }
        if (!(diff & 0xFF00)) {
            P |= CF;
        }
        if (ah & 0x80) {
            ah -= 6;
        }
        A = (ah<<4) | (al & 15);
    }
    else {
        // default mode
        uint16_t diff = A - val - (P & CF ? 0 : 1);
        P &= ~(VF|CF);
        P = YAKC_MOS6502_NZ(P, uint8_t(diff));
        if ((A^val) & (A^diff) & 0x80) {
            P |= VF;
        }
        if (!(diff & 0xFF00)) {
            P |= CF;
        }
        A = diff;
    }
}

//------------------------------------------------------------------------------
inline void
mos6502::sbc() {
    read();
    do_sbc(DATA);
}

//------------------------------------------------------------------------------
inline void
mos6502::u_sbc() {
    read();
    do_sbc(DATA);
}

//------------------------------------------------------------------------------
inline void
mos6502::do_cmp(uint8_t val) {
    uint16_t v = A - val;
    P = YAKC_MOS6502_NZ(P, uint8_t(v)) & ~CF;
    if (!(v & 0xFF00)) {
        P |= CF;
    }
}
//------------------------------------------------------------------------------
inline void
mos6502::cmp() {
    read();
    do_cmp(DATA);
}

//------------------------------------------------------------------------------
inline void
mos6502::cpx() {
    read();
    uint16_t v = X - DATA;
    P = YAKC_MOS6502_NZ(P, uint8_t(v)) & ~CF;
    if (!(v & 0xFF00)) {
        P |= CF;
    }
}

//------------------------------------------------------------------------------
inline void
mos6502::cpy() {
    read();
    uint16_t v = Y - DATA;
    P = YAKC_MOS6502_NZ(P, uint8_t(v)) & ~CF;
    if (!(v & 0xFF00)) {
        P |= CF;
    }
}

//------------------------------------------------------------------------------
inline void
mos6502::dec() {
    read();
    //-- first, the unmodified value is written
    write();
    //-- next cycle the modified value is written
    DATA--; P = YAKC_MOS6502_NZ(P, DATA);
    write();
}

//------------------------------------------------------------------------------
inline void
mos6502::u_dcp() {
    read();
    //-- first, the unmodified value is written
    write();
    //-- next cycle the modified value is written
    DATA--; P = YAKC_MOS6502_NZ(P, DATA);
    write();
    do_cmp(DATA);
}

//------------------------------------------------------------------------------
inline void
mos6502::dex() {
    read();
    X--; P = YAKC_MOS6502_NZ(P, X);
}

//------------------------------------------------------------------------------
inline void
mos6502::dey() {
    read();
    Y--; P = YAKC_MOS6502_NZ(P, Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::inc() {
    read();
    //-- first, the unmodified value is written
    write();
    //-- next cycle the modified value is written
    DATA++; P = YAKC_MOS6502_NZ(P, DATA);
    write();
}

//------------------------------------------------------------------------------
inline void
mos6502::inx() {
    read();
    X++; P = YAKC_MOS6502_NZ(P, X);
}

//------------------------------------------------------------------------------
inline void
mos6502::iny() {
    read();
    Y++; P = YAKC_MOS6502_NZ(P, Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::u_isb() {
    read();
    //-- first, the unmodified value is written
    write();
    //-- next cycle the modified value is written
    DATA++;
    write();
    do_sbc(DATA);
}

//------------------------------------------------------------------------------
inline uint8_t
mos6502::do_asl(uint8_t val) {
    P = (P & ~CF) | ((val & 0x80) ? CF : 0);
    val <<= 1;
    P = YAKC_MOS6502_NZ(P, val);
    return val;
}

//------------------------------------------------------------------------------
inline void
mos6502::asl() {
    read();
    //-- first the unmodified value is written
    write();
    //-- next the modified value is written
    DATA = do_asl(DATA);
    write();
}

//------------------------------------------------------------------------------
inline void
mos6502::asla() {
    read();
    A = do_asl(A);
}

//------------------------------------------------------------------------------
inline void
mos6502::u_slo() {
    read();
    //-- first the unmodified value is written
    write();
    //-- next the modified value is written
    DATA = do_asl(DATA);
    write();
    A |= DATA;
    P = YAKC_MOS6502_NZ(P, A);
}

//------------------------------------------------------------------------------
inline uint8_t
mos6502::do_lsr(uint8_t val) {
    P = (P & ~CF) | ((val & 0x01) ? CF : 0);
    val >>= 1;
    P = YAKC_MOS6502_NZ(P, val);   // N can actually never be set
    return val;
}

//------------------------------------------------------------------------------
inline void
mos6502::lsr() {
    read();
    //-- first the unmodified value is written
    write();
    //-- next the modified value is written
    DATA = do_lsr(DATA);
    write();
}

//------------------------------------------------------------------------------
inline void
mos6502::lsra() {
    read();
    A = do_lsr(A);
}

//------------------------------------------------------------------------------
inline void
mos6502::u_sre() {
    read();
    //-- first the unmodified value is written
    write();
    //-- next the modified value is written
    DATA = do_lsr(DATA);
    write();
    A ^= DATA;
    P = YAKC_MOS6502_NZ(P, A);
}

//------------------------------------------------------------------------------
inline uint8_t
mos6502::do_rol(uint8_t val) {
    bool c = P & CF;
    P &= ~(NF|ZF|CF);
    if (val & 0x80) {
        P |= CF;
    }
    val <<= 1;
    if (c) {
        val |= 0x01;
    }
    P = YAKC_MOS6502_NZ(P, val);
    return val;
}

//------------------------------------------------------------------------------
inline void
mos6502::rol() {
    read();
    //-- first the unmodified value is written
    write();
    //-- next the modified value is written
    DATA = do_rol(DATA);
    write();
}

//------------------------------------------------------------------------------
inline void
mos6502::rola() {
    read();
    A = do_rol(A);
}

//------------------------------------------------------------------------------
inline void
mos6502::u_rla() {
    read();
    //-- first the unmodified value is written
    write();
    //-- next the modified value is written
    DATA = do_rol(DATA);
    write();
    A &= DATA;
    P = YAKC_MOS6502_NZ(P, A);
}

//------------------------------------------------------------------------------
inline uint8_t
mos6502::do_ror(uint8_t val) {
    bool c = P & CF;
    P &= ~(NF|ZF|CF);
    if (val & 0x01) {
        P |= CF;
    }
    val >>= 1;
    if (c) {
        val |= 0x80;
    }
    P = YAKC_MOS6502_NZ(P, val);
    return val;
}

//------------------------------------------------------------------------------
inline void
mos6502::ror() {
    read();
    //-- first the unmodified value is written
    write();
    //-- next the modified value is written
    DATA = do_ror(DATA);
    write();
}

//------------------------------------------------------------------------------
inline void
mos6502::rora() {
    read();
    A = do_ror(A);
}

//------------------------------------------------------------------------------
inline void
mos6502::u_rra() {
    read();
    //-- first the unmodified value is written
    write();
    //-- next the modified value is written
    DATA = do_ror(DATA);
    write();
    do_adc(DATA);
}

//------------------------------------------------------------------------------
inline void
mos6502::bit() {
    read();
    uint8_t v = A & DATA;
    P &= ~(NF|VF|ZF);
    if (!v) {
        P |= ZF;
    }
    P |= DATA & (NF|VF);
}

} // namespace YAKC
