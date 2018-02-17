#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::debugger
    @brief debug helper class
*/
#include "yakc/util/core.h"

namespace YAKC {

class debugger {
    static const int ringbuffer_size = 16;  // must be 2^n
public:
    static const int history_size = ringbuffer_size - 1;
    struct history_item {
        uint16_t pc = 0;
        uint16_t cycles = 0;    // cycles==0 means the item is invalid
    };

    void init(cpu_model m);
    
    void clear_history();
    void add_history_item(uint16_t pc, uint16_t cycles);
    history_item get_history_item(int index);

    void clear_breakpoint();
    void set_breakpoint(uint16_t addr);
    bool is_breakpoint(uint16_t addr);
    bool has_breakpoint();
    bool breakpoint_enabled();
    void toggle_breakpoint(uint16_t addr);
    void enable_breakpoint();
    void disable_breakpoint();
    uint16_t breakpoint_addr();

    void break_trap();
    void break_continue();
    bool break_stopped();

private:
    void set_cpu_trap();
    void clear_cpu_trap();

    cpu_model cpu = cpu_model::z80;
    history_item history[ringbuffer_size];
    int history_pos = 0;
    bool stopped = false;
    bool bp_enabled = false;
    uint16_t bp_addr = 0;
    uint8_t* bp_host_addr = nullptr;
};

} // namespace YAKC
