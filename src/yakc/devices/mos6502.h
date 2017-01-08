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

    bool inv;           // invalid opcode has been hit
    bool irq_active;    // state of IRQ line
    uint32_t cycles;    // current instruction cycles

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

    /// execute a single instruction, return number of cycles
    uint32_t step(system_bus* bus);
    /// fetch next opcode byte
    uint8_t fetch_op();
    /// top-level opcode decoder (generated)
    uint32_t do_op(system_bus* bus);

    /// address mode: immediate
    uint16_t addr_imm();
    /// address mode: absolute
    uint16_t addr_a();
    /// address mode: zero page
    uint16_t addr_z();
    /// address mode: absolute,X/Y
    uint16_t addr_ai(uint8_t i);
    /// address mode: zeropage,X/Y
    uint16_t addr_zi(uint8_t i);
    /// address mode: indirect,X
    uint16_t addr_ix();
    /// address mode: indirect,Y
    uint16_t addr_iy();

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

//------------------------------------------------------------------------------
inline uint8_t
mos6502::fetch_op() {
    return mem.r8(PC++);
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_imm() {
    uint16_t addr = PC++;
    return addr;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_a() {
    uint16_t addr = mem.r16(PC);
    PC += 2;
    return addr;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_z() {
    uint16_t addr = mem.r8(PC++);
    return addr;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_ai(uint8_t i) {
    uint16_t addr = mem.r16(PC) + i;
    PC += 2;
    return addr;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_zi(uint8_t i) {
    uint16_t addr = (mem.r8(PC++) + i) & 0xFF;
    return addr;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_ix() {
    uint16_t addr = mem.r16((mem.r8(PC++) + X) & 0xFF);
    return addr;
}

//------------------------------------------------------------------------------
inline uint16_t
mos6502::addr_iy() {
    uint16_t addr = mem.r16(mem.r8(PC++)) + Y;
    return addr;
}

//------------------------------------------------------------------------------
inline void
mos6502::nop() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::brk() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::lda(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::ldx(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::ldy(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::sta(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::stx(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::sty(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::tax() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::tay() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::txa() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::tya() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::txs() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::tsx() {
    // FIXME
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
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::anda(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::eor(uint16_t addr) {
    // FIXME
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
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::dey() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::inc(uint16_t addr) {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::inx() {
    // FIXME
}

//------------------------------------------------------------------------------
inline void
mos6502::iny() {
    // FIXME
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
