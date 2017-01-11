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

    // current instruction cycle
    int Cycle;
    // ready to fetch and decode next instruction
    bool FetchReady;
    // address mode handling ready
    bool AddrReady;
    // ready to execute instruction
    bool ExecReady;

    // addressing mode codes
    enum {
        A____,
        A_IMM,
        A_ZER,
        A_ZPX,
        A_ZPY,
        A_ABS,
        A_ABX,
        A_ABY,
        A_IDX,
        A_IDY,
    };
    int AddrMode;       // currently active addressing mode

    // addressing mode table
    uint8_t addr_modes[4][8][8];

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

    /// fetch and decode the next instruction
    void step_fetch();
    /// addr state: determine address and put on address bus
    void step_addr();
    /// exec state: execute instruction
    void step_exec(system_bus* bus);
    /// address mode: immediate
    void step_addr_imm();
    /// address mode: zero page
    void step_addr_zer();
    /// address mode: zeropage + X
    void step_addr_zpx();
    /// address mode: zeropage + Y
    void step_addr_zpy();
    /// address mode: absolute
    void step_addr_abs();
    /// address mode: absolute + X
    void step_addr_abx();
    /// address mode: absolute + Y
    void step_addr_aby();
    /// address mode: indexed X
    void step_addr_idx();
    /// address mode: indexed Y
    void step_addr_idy();

    // instructions
    void nop();
    void brk();

    void lda(uint16_t addr);
    void ldx(uint16_t addr);
    void ldy(uint16_t addr);
    void sta(uint16_t addr);
    void stx(uint16_t addr);
    void sty(uint16_t addr);
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
    void jmp(uint16_t addr);
    void jmpi(uint16_t addr);

    void jsr();
    void rts();
    void rti();

    void ora(uint16_t addr);
    void anda(uint16_t addr);
    void eor(uint16_t addr);
    void adc(uint16_t addr);
    void cmp(uint16_t addr);
    void cpx(uint16_t addr);
    void cpy(uint16_t addr);
    void sbc(uint16_t addr);
    void dec(uint16_t addr);
    void dex();
    void dey();
    void inc(uint16_t addr);
    void inx();
    void iny();
    void asl(uint16_t addr);
    void asla();
    void lsr(uint16_t addr);
    void lsra();
    void rol(uint16_t addr);
    void rola();
    void ror(uint16_t addr);
    void rora();
    void bit(uint16_t addr);
};

// set N and Z flags on P depending on V, return new P
#define YAKC_MOS6502_NZ(p,v) ((p & ~(NF|ZF)) | ((v & 0xFF) ? (v & NF):ZF))

//------------------------------------------------------------------------------
inline uint8_t
mos6502::fetch_op() {
    return mem.r8(PC++);
}

//------------------------------------------------------------------------------
inline uint32_t
mos6502::step(system_bus* bus) {
    return do_op(bus);
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_imm() {
    cycles++;
    return PC++;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_a() {
    cycles += 3;
    uint16_t addr = mem.r16(PC);
    PC += 2;
    return addr;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_z() {
    cycles += 2;
    return mem.r8(PC++);
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_ail(uint8_t i) {
    cycles += 3;
    uint16_t a0 = mem.r16(PC);
    PC += 2;
    uint16_t addr = a0 + i;
    if ((a0 & 0xFF00) != (addr & 0xFF00)) {
        // page boundary crossed
        cycles += 1;
    }
    return addr;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_ais(uint8_t i) {
    cycles += 4;
    uint16_t addr = mem.r16(PC) + i;
    PC += 2;
    return addr;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_zi(uint8_t i) {
    cycles += 3;
    return (mem.r8(PC++) + i) & 0x00FF;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_ix() {
    cycles += 5;
    uint16_t z = mem.r8(PC++) + X;
    uint8_t al = mem.r8(z);
    uint8_t ah = mem.r8((z + 1) & 0xFF);
    return ah<<8 | al;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_iyl() {
    cycles += 4;
    uint16_t z = mem.r8(PC++);
    uint8_t al = mem.r8(z);
    uint8_t ah = mem.r8((z + 1) & 0xFF);
    uint16_t a0 = ah<<8 | al;
    uint16_t addr = a0 + Y;
    if ((a0 & 0xFF00) != (addr & 0xFF00)) {
        // page boundary crossed
        cycles += 1;
    }
    return addr;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_iys() {
    cycles += 5;
    uint16_t z = mem.r8(PC++);
    uint8_t al = mem.r8(z);
    uint8_t ah = mem.r8((z + 1) & 0xFF);
    return (ah<<8 | al) + Y;
}

//------------------------------------------------------------------------------
inline void
mos6502::nop() {
    cycles += 1;
}

//------------------------------------------------------------------------------
inline void
mos6502::brk() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::lda(uint16_t addr) {
    A = mem.r8(addr);
    P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::ldx(uint16_t addr) {
    X = mem.r8(addr);
    P = YAKC_MOS6502_NZ(P,X);
}

//------------------------------------------------------------------------------
inline void
mos6502::ldy(uint16_t addr) {
    Y = mem.r8(addr);
    P = YAKC_MOS6502_NZ(P,Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::sta(uint16_t addr) {
    mem.w8(addr, A);
}

//------------------------------------------------------------------------------
inline void
mos6502::stx(uint16_t addr) {
    mem.w8(addr, X);
}

//------------------------------------------------------------------------------
inline void
mos6502::sty(uint16_t addr) {
    mem.w8(addr, Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::tax() {
    cycles += 1;
    X = A;
    P = YAKC_MOS6502_NZ(P,X);
}

//------------------------------------------------------------------------------
inline void
mos6502::tay() {
    cycles += 1;
    Y = A;
    P = YAKC_MOS6502_NZ(P,Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::txa() {
    cycles += 1;
    A = X;
    P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::tya() {
    cycles += 1;
    A = Y;
    P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::txs() {
    cycles += 1;
    S = X;
}

//------------------------------------------------------------------------------
inline void
mos6502::tsx() {
    cycles += 1;
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
mos6502::jmp(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::jmpi(uint16_t addr) {
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
mos6502::ora(uint16_t addr) {
    A |= mem.r8(addr);
    P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::anda(uint16_t addr) {
    A &= mem.r8(addr);
    P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::eor(uint16_t addr) {
    A ^= mem.r8(addr);
    P = YAKC_MOS6502_NZ(P,A);
}

//------------------------------------------------------------------------------
inline void
mos6502::adc(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::cmp(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::cpx(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::cpy(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::sbc(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::dec(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::dex() {
    cycles += 1;
    X--;
    P = YAKC_MOS6502_NZ(P, X);
}

//------------------------------------------------------------------------------
inline void
mos6502::dey() {
    cycles += 1;
    Y--;
    P = YAKC_MOS6502_NZ(P, Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::inc(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::inx() {
    cycles += 1;
    X++;
    P = YAKC_MOS6502_NZ(P, X);
}

//------------------------------------------------------------------------------
inline void
mos6502::iny() {
    cycles += 1;
    Y++;
    P = YAKC_MOS6502_NZ(P, Y);
}

//------------------------------------------------------------------------------
inline void
mos6502::asl(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::asla() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::lsr(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::lsra() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::rol(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::rola() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::ror(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::rora() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::bit(uint16_t addr) {
    // FIXME
}

} // namespace YAKC
