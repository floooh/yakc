#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::cpudbg
    @brief debug helper functions
*/
#include "yakc/util/core.h"

namespace YAKC {

class cpudbg {
private:
    /// must be 2^N!
    static const int ringbuffer_size = 16;
public:
    /// number of valid PC history items
    static const int history_size = ringbuffer_size-1;
    /// an item in the history
    struct history_item {
        uint16_t pc = 0;
        uint16_t cycles = 0;
    };
    /// debugger is currently active
    bool active = false;

    /// constructor
    cpudbg();
    /// check if breakpoint hit, and store pc in history, return if breakkpoint hit
    bool step(uint16_t pc, uint16_t op_cycles);
    /// get pc from history ringbuffer (0 is oldest entry)
    history_item get_pc_history(int index) const;
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

private:
    int hist_pos = 0;
    history_item history[ringbuffer_size];
    struct breakpoint {
        breakpoint() : enabled(false), address(0) {};
        bool enabled;
        uint16_t address;
    } bp;
};

//------------------------------------------------------------------------------
inline bool
cpudbg::step(uint16_t pc, uint16_t op_cycles) {
    // store pc in history
    history[hist_pos].cycles = op_cycles;
    hist_pos = (this->hist_pos+1) & (ringbuffer_size-1);
    history[hist_pos].pc = pc;
    history[hist_pos].cycles = 0;

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
