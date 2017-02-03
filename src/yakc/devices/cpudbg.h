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
private:
public:
    /// size of PC history ringbuffer, must be 2^N!
    static const int pc_history_size = 8;
    /// debugger is currently active
    bool active = false;

    /// constructor
    cpudbg();

    /// check if breakpoint hit, and store pc in history, return if breakkpoint hit
    bool step(uint16_t pc, uint8_t op_cycles);
    
    /// get pc from history ringbuffer (0 is oldest entry)
    uint16_t get_pc_history(int index) const;

    /// enable breakpoint
    void enable_breakpoint(uint16_t addr);
    /// disable breakpoint
    void disable_breakpoint();
    /// if breakpoint at addr exists, toggle state, else enable breakpoint
    void toggle_breakpoint(uint16_t addr);
    /// test if an address is breakpoint
    bool is_breakpoint(uint16_t addr) const;
    /// return true if breakpoint is enabled
    bool breakpoint_enabled() const;
    /// get breakpoint address
    uint16_t breakpoint_addr() const;

    /// step until PC changed (or an invalid opcode is hit)
    void step_pc_modified(system_bus* bus, z80& cpu);
    /// step until PC changed (or an invalid opcode is hit)
    void step_pc_modified(mos6502& cpu);

private:
    int pc_history_pos = 0;
    uint16_t pc_history[pc_history_size];
    struct breakpoint {
        breakpoint() : enabled(false), address(0) {};
        bool enabled;
        uint16_t address;
    } bp;
};

//------------------------------------------------------------------------------
inline bool
cpudbg::step(uint16_t pc, uint8_t op_cycles) {
    // store pc in history
    this->pc_history[pc_history_pos++] = pc;
    this->pc_history_pos &= pc_history_size-1;

    // check breakpoint
    if (this->bp.enabled) {
        if (pc == this->bp.address) {
            this->active = true;
            return true;
        }
    }
    return false;
}

} // namespace YAKC
