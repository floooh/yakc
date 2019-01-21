//------------------------------------------------------------------------------
//  debugger.cc
//------------------------------------------------------------------------------
#include "debugger.h"
#include "yakc/util/breadboard.h"

namespace YAKC {

//------------------------------------------------------------------------------
void 
debugger::init(cpu_model c) {
    this->cpu = c;
    this->stopped = false;
    this->clear_history();
    this->bp_enabled = false;
    this->bp_addr = 0;
}

//------------------------------------------------------------------------------
void
debugger::clear_history() {
    this->history_pos = 0;
    for (auto& item : this->history) {
        item = history_item();
    }
}

//------------------------------------------------------------------------------
void
debugger::add_history_item(uint16_t pc, uint16_t cycles) {
    this->history[this->history_pos].cycles = cycles;
    this->history_pos = (this->history_pos+1) & (ringbuffer_size-1);
    this->history[this->history_pos].valid = true;
    this->history[this->history_pos].pc = pc;
    this->history[this->history_pos].cycles = 0;
}

//------------------------------------------------------------------------------
debugger::history_item
debugger::get_history_item(int index) {
    int i = ((this->history_pos - history_size) + index) & (ringbuffer_size-1);
    return this->history[i];
}

//------------------------------------------------------------------------------
void
debugger::enable_breakpoint(uint16_t addr) {
    this->bp_addr = addr;
    this->bp_enabled = true;
}

//------------------------------------------------------------------------------
void
debugger::disable_breakpoint() {
    this->bp_enabled = false;
}

//------------------------------------------------------------------------------
bool
debugger::is_breakpoint(uint16_t addr) {
    return this->bp_enabled && (addr == this->bp_addr);
}

//------------------------------------------------------------------------------
bool
debugger::breakpoint_enabled() {
    return this->bp_enabled;
}

//------------------------------------------------------------------------------
void
debugger::toggle_breakpoint(uint16_t addr) {
    if (this->bp_enabled) {
        if (addr == this->bp_addr) {
            this->disable_breakpoint();
        }
        else {
            this->enable_breakpoint(addr);
        }
    }
    else {
        this->enable_breakpoint(addr);
    }
}

//------------------------------------------------------------------------------
uint16_t
debugger::breakpoint_addr() {
    return this->bp_addr;
}

//------------------------------------------------------------------------------
void
debugger::break_stop() {
    if (!this->stopped) {
        this->stopped = true;
    }
}

//------------------------------------------------------------------------------
void
debugger::break_check() {
    if (this->cpu == cpu_model::z80) {
        if (board.z80->trap_id == 0) {
            this->break_stop();
        }
    }
    else {
        if (board.m6502->trap_id == 0) {
            this->break_stop();
        }
    }
}

//------------------------------------------------------------------------------
void
debugger::break_continue() {
    if (this->stopped) {
        this->stopped = false;
    }
}

//------------------------------------------------------------------------------
bool
debugger::break_stopped() {
    return this->stopped;
}

} // namespace YAKC
