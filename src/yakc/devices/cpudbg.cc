//------------------------------------------------------------------------------
//  cpudbg.cc
//------------------------------------------------------------------------------
#include "cpudbg.h"

namespace YAKC {

//------------------------------------------------------------------------------
cpudbg::cpudbg() :
pc_history_pos(0),
paused(false) {
    memset(&this->pc_history, 0, sizeof(this->pc_history));
}

//------------------------------------------------------------------------------
bool
cpudbg::check_break(uint16_t pc) const {
    for (int i = 0; i < max_breakpoints; i++) {
        if (this->breakpoints[i].enabled) {
            if (pc == this->breakpoints[i].address) {
                return true;
            }
        }
    }
    return false;
}

//------------------------------------------------------------------------------
void
cpudbg::store_pc_history(uint16_t pc) {
    this->pc_history[pc_history_pos++] = pc;
    this->pc_history_pos &= this->pc_history_size-1;
}

//------------------------------------------------------------------------------
uint16_t
cpudbg::get_pc_history(int index) const {
    int i = ((this->pc_history_pos-pc_history_size)+index) & (pc_history_size-1);
    return pc_history[i];
}

//------------------------------------------------------------------------------
void
cpudbg::enable_breakpoint(int index, uint16_t addr) {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    this->breakpoints[index].enabled = true;
    this->breakpoints[index].address = addr;
}

//------------------------------------------------------------------------------
void
cpudbg::disable_breakpoint(int index) {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    this->breakpoints[index].enabled = false;
}

//------------------------------------------------------------------------------
void
cpudbg::toggle_breakpoint(int index, uint16_t addr) {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    if (this->breakpoints[index].address == addr) {
        this->breakpoints[index].enabled = !this->breakpoints[index].enabled;
    }
    else {
        this->enable_breakpoint(index, addr);
    }
}

//------------------------------------------------------------------------------
bool
cpudbg::is_breakpoint(uint16_t addr) const {
    for (int i = 0; i < max_breakpoints; i++) {
        if ((this->breakpoints[i].enabled) && (this->breakpoints[i].address == addr)) {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------------------------
bool
cpudbg::breakpoint_enabled(int index) const {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    return this->breakpoints[index].enabled;
}

//------------------------------------------------------------------------------
uint16_t
cpudbg::breakpoint_addr(int index) const {
    YAKC_ASSERT((index >= 0) && (index < max_breakpoints));
    return this->breakpoints[index].address;
}

//------------------------------------------------------------------------------
void
cpudbg::step_pc_modified(system_bus* bus, z80& cpu) {
    YAKC_ASSERT(bus);
    uint16_t pc;
    do {
        pc = cpu.PC;
        this->store_pc_history(pc);
        cpu.step(bus);
    }
    while ((pc == cpu.PC) && !cpu.INV);
}

//------------------------------------------------------------------------------
void
cpudbg::step_pc_modified(mos6502& cpu) {
    uint16_t pc;
    do {
        pc = cpu.PC;
        this->store_pc_history(pc);
        cpu.step();
    }
    while (pc == cpu.PC);
}

} // namespace YAKC
