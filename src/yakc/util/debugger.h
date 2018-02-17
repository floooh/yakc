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

    void clear();
    void add_history_item(uint16_t pc, uint16_t cycles);
    void clear_history();
    history_item get_history_item(int index) const;
    void set_breakpoint(cpu_model m, uint16_t addr, uint8_t* host_addr);
    void clear_breakpoint(cpu_model m);
    bool has_breakpoint(cpu_model m) const;
    uint16_t breakpoint_addr(cpu_model m);

    int history_pos = 0;
    history_item history[ringbuffer_size];
};

} // namespace YAKC
