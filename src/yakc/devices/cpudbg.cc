//------------------------------------------------------------------------------
//  cpudbg.cc
//------------------------------------------------------------------------------
#include "cpudbg.h"

namespace YAKC {

//------------------------------------------------------------------------------
cpudbg::cpudbg() {
    memset(&this->pc_history, 0, sizeof(this->pc_history));
}

//------------------------------------------------------------------------------
uint16_t
cpudbg::get_pc_history(int index) const {
    int i = ((this->pc_history_pos-pc_history_size)+index) & (pc_history_size-1);
    return pc_history[i];
}

//------------------------------------------------------------------------------
void
cpudbg::enable_breakpoint(uint16_t addr) {
    this->bp.enabled = true;
    this->bp.address = addr;
}

//------------------------------------------------------------------------------
void
cpudbg::disable_breakpoint() {
    this->bp.enabled = false;
}

//------------------------------------------------------------------------------
void
cpudbg::toggle_breakpoint(uint16_t addr) {
    if (this->bp.address == addr) {
        this->bp.enabled = !this->bp.enabled;
    }
    else {
        this->enable_breakpoint(addr);
    }
}

//------------------------------------------------------------------------------
bool
cpudbg::is_breakpoint(uint16_t addr) const {
    if ((this->bp.enabled) && (this->bp.address == addr)) {
        return true;
    }
    else {
        return false;
    }
}

//------------------------------------------------------------------------------
bool
cpudbg::breakpoint_enabled() const {
    return this->bp.enabled;
}

//------------------------------------------------------------------------------
uint16_t
cpudbg::breakpoint_addr() const {
    return this->bp.address;
}

//------------------------------------------------------------------------------
void
cpudbg::step_pc_modified(system_bus* bus, z80& cpu) {
    YAKC_ASSERT(bus);
    uint16_t old_pc;
    do {
        old_pc = cpu.PC;
        uint32_t ticks = cpu.step(bus);
        this->step(cpu.PC, ticks);
    }
    while ((old_pc == cpu.PC) && !cpu.INV);
}

//------------------------------------------------------------------------------
void
cpudbg::step_pc_modified(mos6502& cpu) {
    uint16_t old_pc;
    do {
        old_pc = cpu.PC;
        uint32_t ticks = cpu.step();
        this->step(cpu.PC, ticks);
    }
    while (old_pc == cpu.PC);
}

} // namespace YAKC
