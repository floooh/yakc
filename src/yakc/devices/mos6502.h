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

    /// read or write next byte, incr cycle, tick bus
    void rw(bool read);
    /// fetch and decode the next instruction
    void fetch();
    /// determine address and put on address bus
    void addr();
    /// execute instruction
    void exec();
    
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
};

// set N and Z flags on P depending on V, return new P
#define YAKC_MOS6502_NZ(p,v) ((p & ~(NF|ZF)) | ((v & 0xFF) ? (v & NF):ZF))

//------------------------------------------------------------------------------
inline void
mos6502::brk() {
    YAKC_ASSERT(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::nop() {
    rw(true);
}

//------------------------------------------------------------------------------
inline void
mos6502::u_nop() {
    rw(true);
}

//------------------------------------------------------------------------------
inline void
mos6502::lda() {
    rw(true);
    A = DATA; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::ldx() {
    rw(true);
    X = DATA; P = YAKC_MOS6502_NZ(P,X);
}

//------------------------------------------------------------------------------
inline void
mos6502::ldy() {
    rw(true);
    Y = DATA; P = YAKC_MOS6502_NZ(P,Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::u_lax() {
    rw(true);
    A = X = DATA; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::sta() {
    DATA = A;
    rw(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::stx() {
    DATA = X;
    rw(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::sty() {
    DATA = Y;
    rw(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::u_sax() {
    DATA = A & X;
    rw(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::tax() {
    rw(true);
    X = A; P = YAKC_MOS6502_NZ(P,X);
}

//------------------------------------------------------------------------------
inline void
mos6502::tay() {
    rw(true);
    Y = A; P = YAKC_MOS6502_NZ(P,Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::txa() {
    rw(true);
    A = X; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::tya() {
    rw(true);
    A = Y; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::txs() {
    rw(true);
    S = X;
}

//------------------------------------------------------------------------------
inline void
mos6502::tsx() {
    rw(true);
    X = S; P = YAKC_MOS6502_NZ(P,X);
}

//------------------------------------------------------------------------------
inline void
mos6502::php() {
    rw(true);
    //--
    ADDR = 0x0100|S--; DATA = (P | BF);
    rw(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::plp() {
    rw(true);
    //-- first read junk from current SP
    ADDR = 0x0100|S++;
    rw(true);
    //-- read actual byte
    ADDR = 0x0100|S;
    rw(true);
    P = (DATA & ~BF) | XF;
}

//------------------------------------------------------------------------------
inline void
mos6502::pha() {
    rw(true);
    //--
    ADDR = 0x0100|S--; DATA = A;
    rw(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::pla() {
    rw(true);
    //-- first read junk from current SP
    ADDR = 0x0100|S++;
    rw(true);
    // read actual byte
    ADDR = 0x0100|S;
    rw(true);
    A = DATA; P = YAKC_MOS6502_NZ(P, A);
}

//------------------------------------------------------------------------------
inline void
mos6502::se(uint8_t f) {
    rw(true);
    P |= f;
}

//------------------------------------------------------------------------------
inline void
mos6502::cl(uint8_t f) {
    rw(true);
    P &= ~f;
}

//------------------------------------------------------------------------------
inline void
mos6502::br(uint8_t m, uint8_t v) {
    rw(true);
    // check if the branch is taken, if not return after 2 cycles
    if ((P & m) != v) {
        return;
    }
    //--
    rw(true);
    // branch was taken, compute target address, return after 3
    // cycles if target address is in same 256 bytes page
    tmp16 = PC + int8_t(DATA);
    if ((tmp16 & 0xFF00) == (PC & 0xFF00)) {
        PC = tmp16;
        return;
    }
    rw(true);
    // page boundary was crossed, return after 4 cycles
    PC = tmp16;
}

//------------------------------------------------------------------------------
inline void
mos6502::jmp() {
    rw(true);
    PC = (DATA<<8) | (tmp16 & 0x00FF);
}

//------------------------------------------------------------------------------
inline void
mos6502::jmpi() {
    rw(true);
    // load first byte of target address
    ADDR = (DATA<<8) | (tmp16 & 0x00FF);
    rw(true);
    // load second byte of target address, wrap around in current page
    tmp16 = DATA;
    ADDR = (ADDR & 0xFF00) | ((ADDR + 1) & 0x00FF);
    rw(true);
    // form target address in PC
    PC = (DATA<<8) | (tmp16 & 0x00FF);
}

//------------------------------------------------------------------------------
inline void
mos6502::jsr() {
    rw(true);
    // low-byte of target address loaded, and put SP
    // on addr bus, but the next cycle is a junk read
    tmp16 = DATA; ADDR = 0x0100 | S;
    rw(true);
    // write PC high byte to stack
    DATA = PC >> 8;
    ADDR = 0x0100 | S--;
    rw(false);
    // write PC low byte to stack
    ADDR = 0x0100 | S--;
    DATA = PC;
    rw(false);
    // load the target address high-byte
    ADDR = PC;
    rw(true);
    // and finally jump to target address
    PC = (DATA<<8) | (tmp16 & 0x00FF);
}

//------------------------------------------------------------------------------
inline void
mos6502::rts() {
    rw(true);
    // first put SP on addr bus and do a junk read
    ADDR = 0x0100 | S++;
    rw(true);
    // read return addr low byte
    ADDR = 0x0100 | S++;
    rw(true);
    // keep return addr low byte, and read high byte from stack
    tmp16 = DATA; ADDR = 0x0100 | S;
    rw(true);
    // put return address in PC, this is one byte before next op, do a junk read
    PC = (DATA<<8) | (tmp16 & 0x00FF);
    ADDR = PC++;
    rw(true);
}

//------------------------------------------------------------------------------
inline void
mos6502::rti() {
    rw(true);
    // first put SP on addr bus and do a junk read
    ADDR = 0x0100 | S++;
    rw(true);
    // read processor status flag
    ADDR = 0x0100 | S++;
    rw(true);
    // store P, read return addr low byte
    P = (DATA & ~BF) | XF; ADDR = 0x0100 | S++;
    rw(true);
    // store return-addr low byte, read high byte
    tmp16 = DATA; ADDR = 0x0100 | S;
    rw(true);
    // update PC
    PC = (DATA<<8) | (tmp16 & 0x00FF);
}

//------------------------------------------------------------------------------
inline void
mos6502::ora() {
    rw(true);
    A |= DATA; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::anda() {
    rw(true);
    A &= DATA; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::eor() {
    rw(true);
    A ^= DATA; P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::adc() {
    rw(true);
    // from MAME
    if (bcd_enabled && (P & DF)) {
        // decimal mode
        uint8_t c  = P & CF ? 1 : 0;
        P &= ~(NF|VF|ZF|CF);
        uint8_t al = (A & 0x0F) + (DATA & 0x0F) + c;
        if (al > 9) {
            al += 6;
        }
        uint8_t ah = (A >> 4) + (DATA >> 4) + (al > 0x0F);
        if (0 == (A + DATA + c)) {
            P |= ZF;
        }
        else if (ah & 0x08) {
            P |= NF;
        }
        if (~(A^DATA) & (A^(ah<<4)) & 0x80) {
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
        uint16_t sum = A + DATA + (P & CF ? 1:0);
        P &= ~(VF|CF);
        P = YAKC_MOS6502_NZ(P, uint8_t(sum));
        if (~(A^DATA) & (A^sum) & 0x80) {
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
mos6502::sbc() {
    rw(true);
    // from MAME
    if (bcd_enabled && (P & DF)) {
        // decimal mode
        uint8_t c = P & CF ? 0 : 1;
        P &= ~(NF|VF|ZF|CF);
        uint16_t diff = A - DATA - c;
        uint8_t al = (A & 0x0F) - (DATA & 0x0F) - c;
        if (int8_t(al) < 0) {
            al -= 6;
        }
        uint8_t ah = (A>>4) - (DATA>>4) - (int8_t(al) < 0);
        if (0 == uint8_t(diff)) {
            P |= ZF;
        }
        else if (diff & 0x80) {
            P |= NF;
        }
        if ((A^DATA) & (A^diff) & 0x80) {
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
        uint16_t diff = A - DATA - (P & CF ? 0 : 1);
        P &= ~(VF|CF);
        P = YAKC_MOS6502_NZ(P, uint8_t(diff));
        if ((A^DATA) & (A^diff) & 0x80) {
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
mos6502::u_sbc() {
    sbc();
}

//------------------------------------------------------------------------------
inline void
mos6502::cmp() {
    rw(true);
    uint16_t v = A - DATA;
    P = YAKC_MOS6502_NZ(P, uint8_t(v)) & ~CF;
    if (!(v & 0xFF00)) {
        P |= CF;
    }
}

//------------------------------------------------------------------------------
inline void
mos6502::cpx() {
    rw(true);
    uint16_t v = X - DATA;
    P = YAKC_MOS6502_NZ(P, uint8_t(v)) & ~CF;
    if (!(v & 0xFF00)) {
        P |= CF;
    }
}

//------------------------------------------------------------------------------
inline void
mos6502::cpy() {
    rw(true);
    uint16_t v = Y - DATA;
    P = YAKC_MOS6502_NZ(P, uint8_t(v)) & ~CF;
    if (!(v & 0xFF00)) {
        P |= CF;
    }
}

//------------------------------------------------------------------------------
inline void
mos6502::dec() {
    rw(true);
    //-- first, the unmodified value is written
    rw(false);
    //-- next cycle the modified value is written
    DATA--; P = YAKC_MOS6502_NZ(P, DATA);
    rw(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::u_dcp() {
    rw(true);
    //-- first, the unmodified value is written
    rw(false);
    //-- next cycle the modified value is written
    DATA--; P = YAKC_MOS6502_NZ(P, DATA);
    rw(false);
    // update flags
    uint16_t v = A - DATA;
    P = YAKC_MOS6502_NZ(P, uint8_t(v)) & ~CF;
    if (!(v & 0xFF00)) {
        P |= CF;
    }
}

//------------------------------------------------------------------------------
inline void
mos6502::dex() {
    rw(true);
    X--; P = YAKC_MOS6502_NZ(P, X);
}

//------------------------------------------------------------------------------
inline void
mos6502::dey() {
    rw(true);
    Y--; P = YAKC_MOS6502_NZ(P, Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::inc() {
    rw(true);
    //-- first, the unmodified value is written
    rw(false);
    //-- next cycle the modified value is written
    DATA++; P = YAKC_MOS6502_NZ(P, DATA);
    rw(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::inx() {
    rw(true);
    X++; P = YAKC_MOS6502_NZ(P, X);
}

//------------------------------------------------------------------------------
inline void
mos6502::iny() {
    rw(true);
    Y++; P = YAKC_MOS6502_NZ(P, Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::asl() {
    rw(true);
    //-- first the unmodified value is written
    rw(false);
    //-- next the modified value is written
    P = (P & ~CF) | ((DATA & 0x80) ? CF : 0);
    DATA <<= 1;
    P = YAKC_MOS6502_NZ(P, DATA);
    rw(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::asla() {
    rw(true);
    P = (P & ~CF) | ((A & 0x80) ? CF : 0);
    A <<= 1;
    P = YAKC_MOS6502_NZ(P, A);
}

//------------------------------------------------------------------------------
inline void
mos6502::lsr() {
    rw(true);
    //-- first the unmodified value is written
    rw(false);
    //-- next the modified value is written
    P = (P & ~CF) | ((DATA & 0x01) ? CF : 0);
    DATA >>= 1;
    P = YAKC_MOS6502_NZ(P, DATA);   // N can actually never be set
    rw(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::lsra() {
    rw(true);
    P = (P & ~CF) | ((A & 0x01) ? CF : 0);
    A >>= 1;
    P = YAKC_MOS6502_NZ(P, A);   // N can actually never be set
}

//------------------------------------------------------------------------------
inline void
mos6502::rol() {
    rw(true);
    //-- first the unmodified value is written
    rw(false);
    //-- next the modified value is written
    bool c = P & CF;
    P &= ~(NF|ZF|CF);
    if (DATA & 0x80) {
        P |= CF;
    }
    DATA <<= 1;
    if (c) {
        DATA |= 0x01;
    }
    P = YAKC_MOS6502_NZ(P, DATA);
    rw(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::rola() {
    rw(true);
    bool c = P & CF;
    P &= ~(NF|ZF|CF);
    if (A & 0x80) {
        P |= CF;
    }
    A <<= 1;
    if (c) {
        A |= 0x01;
    }
    P = YAKC_MOS6502_NZ(P, A);
}

//------------------------------------------------------------------------------
inline void
mos6502::ror() {
    rw(true);
    //-- first the unmodified value is written
    rw(false);
    //-- next the modified value is written
    bool c = P & CF;
    P &= ~(NF|ZF|CF);
    if (DATA & 0x01) {
        P |= CF;
    }
    DATA >>= 1;
    if (c) {
        DATA |= 0x80;
    }
    P = YAKC_MOS6502_NZ(P, DATA);
    rw(false);
}

//------------------------------------------------------------------------------
inline void
mos6502::rora() {
    rw(true);
    bool c = P & CF;
    P &= ~(NF|ZF|CF);
    if (A & 0x01) {
        P |= CF;
    }
    A >>= 1;
    if (c) {
        A |= 0x80;
    }
    P = YAKC_MOS6502_NZ(P, A);
}

//------------------------------------------------------------------------------
inline void
mos6502::bit() {
    rw(true);
    uint8_t v = A & DATA;
    P &= ~(NF|VF|ZF);
    if (!v) {
        P |= ZF;
    }
    P |= DATA & (NF|VF);
}

} // namespace YAKC
