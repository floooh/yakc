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

    // registers
    uint8_t A,X,Y,S,P;
    uint16_t PC;
    uint8_t OP;

    // pins
    bool RW;            // read: true, write: false
    uint16_t ADDR;      // current address bus value
    uint8_t DATA;       // current data bus value
    uint16_t tmp;

    int Cycle;              // current instruction cycle
    bool Fetch;             // true: currently in fetch/decode cycle
    int AddrCycle;          // > 0: currently in address mode state
    int ExecCycle;          // > 0: currently in execute state
    uint8_t AddrMode;       // currently active addressing mode
    uint8_t MemAccess;      // M_R, M_W, M_RW

    // addressing mode table
    static uint8_t addr_modes[4][8][8];
    // memory access table
    static uint8_t rw_table[4][8];

    // memory map
    memory mem;

    /// constructor
    mos6502();

    /// one-time init
    void init();
    /// reset the CPU
    void reset();
    /// called when an invalid opcode has been hit
    uint32_t invalid_opcode();

    /// trigger interrupt request line on/off
    void irq(bool b);
    /// handle an interrupt request
    int handle_irq(system_bus* bus);

    /// execute a single cycle
    void step(system_bus* bus);
    /// execute a single instruction, return cycles
    uint32_t step_op(system_bus* bus);

    /// prepare to fetch next instruction
    void step_fetch();
    /// decode fetched instruction (same cycle as fetch)
    void step_decode();
    /// addr state: determine address and put on address bus
    void step_addr();
    /// exec state: execute instruction
    void step_exec();
    
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
};

// set N and Z flags on P depending on V, return new P
#define YAKC_MOS6502_NZ(p,v) ((p & ~(NF|ZF)) | ((v & 0xFF) ? (v & NF):ZF))

//------------------------------------------------------------------------------
inline void
mos6502::brk() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::nop() {
    // nop
}

//------------------------------------------------------------------------------
inline void
mos6502::lda() {
    A = DATA;
    P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::ldx() {
    X = DATA;
    P = YAKC_MOS6502_NZ(P,X);
}

//------------------------------------------------------------------------------
inline void
mos6502::ldy() {
    Y = DATA;
    P = YAKC_MOS6502_NZ(P,Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::sta() {
    DATA = A;
}

//------------------------------------------------------------------------------
inline void
mos6502::stx() {
    DATA = X;
}

//------------------------------------------------------------------------------
inline void
mos6502::sty() {
    DATA = Y;
}

//------------------------------------------------------------------------------
inline void
mos6502::tax() {
    X = A;
    P = YAKC_MOS6502_NZ(P,X);
}

//------------------------------------------------------------------------------
inline void
mos6502::tay() {
    Y = A;
    P = YAKC_MOS6502_NZ(P,Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::txa() {
    A = X;
    P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::tya() {
    A = Y;
    P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::txs() {
    S = X;
}

//------------------------------------------------------------------------------
inline void
mos6502::tsx() {
    X = S;
    P = YAKC_MOS6502_NZ(P,X);
}

//------------------------------------------------------------------------------
inline void
mos6502::php() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::plp() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::pha() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::pla() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::se(uint8_t f) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::cl(uint8_t f) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::br(uint8_t m, uint8_t v) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::jmp() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::jmpi() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::jsr() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::rts() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::rti() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::ora() {
    A |= DATA;
    P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::anda() {
    A &= DATA;
    P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::eor() {
    A ^= DATA;
    P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::adc() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::cmp() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::cpx() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::cpy() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::sbc() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::dec() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::dex() {
    X--;
    P = YAKC_MOS6502_NZ(P, X);
}

//------------------------------------------------------------------------------
inline void
mos6502::dey() {
    Y--;
    P = YAKC_MOS6502_NZ(P, Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::inc() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::inx() {
    X++;
    P = YAKC_MOS6502_NZ(P, X);
}

//------------------------------------------------------------------------------
inline void
mos6502::iny() {
    Y++;
    P = YAKC_MOS6502_NZ(P, Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::asl() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::asla() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::lsr() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::lsra() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::rol() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::rola() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::ror() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::rora() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::bit() {
    // FIXME
}

} // namespace YAKC
