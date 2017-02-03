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

private:
    static const int max_breakpoints = 2;
    struct breakpoint {
        breakpoint() : enabled(false), address(0) {};
        bool enabled;
        uint16_t address;
    } breakpoints[max_breakpoints];
};

} // namespace YAKC
