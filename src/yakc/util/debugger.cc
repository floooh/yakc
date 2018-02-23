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
    this->clear_breakpoint();
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
debugger::set_cpu_trap() {
    if (this->cpu == cpu_model::z80) {
        z80_set_trap(&board.z80, 0, this->bp_addr, this->bp_host_addr);
    }
    else {
        m6502_set_trap(&board.m6502, 0, this->bp_addr, this->bp_host_addr); 
    }
}

//------------------------------------------------------------------------------
void
debugger::clear_cpu_trap() {
    if (this->cpu == cpu_model::z80) {
        if (z80_has_trap(&board.z80, 0)) {
            z80_clear_trap(&board.z80, 0);
        }
    }
    else {
        if (m6502_has_trap(&board.m6502, 0)) {
            m6502_clear_trap(&board.m6502, 0);
        }
    }
}

//------------------------------------------------------------------------------
void
debugger::clear_breakpoint() {
    if (!this->stopped) {
        this->clear_cpu_trap();
    }
    this->bp_enabled = false;
    this->bp_addr = 0;
    this->bp_host_addr = nullptr;
}

//------------------------------------------------------------------------------
void
debugger::set_breakpoint(uint16_t addr) {
    this->bp_addr = addr;
    this->bp_host_addr = mem_readptr(&board.mem, addr);
    this->enable_breakpoint();
}

//------------------------------------------------------------------------------
bool
debugger::is_breakpoint(uint16_t addr) {
    return this->has_breakpoint() && (addr == this->bp_addr);
}

//------------------------------------------------------------------------------
bool
debugger::has_breakpoint() {
    return this->bp_host_addr != nullptr;
}

//------------------------------------------------------------------------------
bool
debugger::breakpoint_enabled() {
    return this->bp_enabled;
}

//------------------------------------------------------------------------------
void
debugger::toggle_breakpoint(uint16_t addr) {
    if (this->is_breakpoint(addr)) {
        this->clear_breakpoint();
    }
    else {
        this->set_breakpoint(addr);
    }
}

//------------------------------------------------------------------------------
void
debugger::enable_breakpoint() {
    if (this->has_breakpoint()) {
        this->bp_enabled = true;
        if (!this->stopped) {
            this->set_cpu_trap();
        }
    }
}

//------------------------------------------------------------------------------
void
debugger::disable_breakpoint() {
    if (this->has_breakpoint()) {
        this->bp_enabled = false;
        if (!this->stopped) {
            this->clear_cpu_trap();
        }
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
        if (this->has_breakpoint()) {
            this->clear_cpu_trap();
        }
    }
}

//------------------------------------------------------------------------------
void
debugger::break_check() {
    if (this->cpu == cpu_model::z80) {
        if (board.z80.trap_id == 0) {
            this->break_stop();
        }
    }
    else {
        if (board.m6502.trap_id == 0) {
            this->break_stop();
        }
    }
}

//------------------------------------------------------------------------------
void
debugger::break_continue() {
    if (this->stopped) {
        this->stopped = false;
        if (this->has_breakpoint()) {
            this->set_cpu_trap();
        }
    }
}

//------------------------------------------------------------------------------
bool
debugger::break_stopped() {
    return this->stopped;
}

} // namespace YAKC
