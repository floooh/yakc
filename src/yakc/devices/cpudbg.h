#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::cpudbg
    @brief debug helper functions
*/
#include "yakc/core/core.h"
#include "yakc/devices/z80.h"
#include "yakc/devices/mos6502.h"

namespace YAKC {

class cpudbg {
public:
    /// register enum for set_reg/get_reg methods
    enum class z80reg {
        A=0, F, B, C, D, E, H, L,
        AF, BC, DE, HL, WZ,
        AF_, BC_, DE_, HL_, WZ_,
        I, R, IX, IY, SP, PC,
        IM,
        num
    };
    enum class m6502reg {
        A=0, X, Y, S, P, PC,
        num
    };

    /// size of PC history ringbuffer (must be 2^N!)
    static const int pc_history_size = 8;
    /// current pc history position
    int pc_history_pos;
    /// a history ringbuffer of previous PC addresses
    uint16_t pc_history[pc_history_size];
    /// execution paused (e.g. because in debugger)
    bool paused;

    /// constructor
    cpudbg();

    /// test whether breakpoint is enabled and hit
    bool check_break(uint16_t pc) const;
    /// store current pc in history ringbuffer
    void store_pc_history(uint16_t pc);
    /// get pc from history ringbuffer (0 is oldest entry)
    uint16_t get_pc_history(int index) const;

    /// enable a breakpoint
    void enable_breakpoint(int index, uint16_t addr);
    /// disable a breakpoint
    void disable_breakpoint(int index);
    /// if breakpoint at addr exists, toggle state, else enable breakpoint
    void toggle_breakpoint(int index, uint16_t addr);
    /// test if an address is a breakpoint
    bool is_breakpoint(uint16_t addr) const;
    /// return true if breakpoint is enabled
    bool breakpoint_enabled(int index) const;
    /// get breakpoint address
    uint16_t breakpoint_addr(int index) const;

    /// step until PC changed (or an invalid opcode is hit)
    void step_pc_modified(system_bus* bus, z80& cpu);
    /// step until PC changed (or an invalid opcode is hit)
    void step_pc_modified(mos6502& cpu);

    /// set a Z80 8-bit register value by enum (slow)
    static void set8(z80& cpu, z80reg r, uint8_t v);
    /// get a Z80 8-bit register value by enum (slow)
    static uint8_t get8(const z80& cpu, z80reg r);
    /// set a Z80 16-bit register value by enum (slow)
    static void set16(z80& cpu, z80reg r, uint16_t v);
    /// get a Z80 16-bit register value by enum (slow)
    static uint16_t get16(const z80& cpu, z80reg r);
    /// get a string-name for a Z80 register
    static const char* reg_name(z80reg r);

    /// set a 6502 8-bit register value by enum (slow)
    static void set8(mos6502& cpu, m6502reg r, uint8_t v);
    /// get a 6502 8-bit register value by enum (slow)
    static uint8_t get8(const mos6502& cpu, m6502reg r);
    /// set a 6502 16-bit register value by enum (slow)
    static void set16(mos6502& cpu, m6502reg r, uint16_t v);
    /// get a 6502 16-bit register value by enum (slow)
    static uint16_t get16(const mos6502& cpu, m6502reg r);
    /// get a string-name for a register
    static const char* reg_name(m6502reg r);

private:
    static const int max_breakpoints = 2;
    struct breakpoint {
        breakpoint() : enabled(false), address(0) {};
        bool enabled;
        uint16_t address;
    } breakpoints[max_breakpoints];
};

} // namespace YAKC
