//------------------------------------------------------------------------------
//  cpudbg.cc
//------------------------------------------------------------------------------
#include "cpudbg.h"

namespace YAKC {

//------------------------------------------------------------------------------
cpudbg::cpudbg() {
    memset(&this->history, 0, sizeof(this->history));
}

//------------------------------------------------------------------------------
cpudbg::history_item
cpudbg::get_pc_history(int index) const {
    int i = ((this->hist_pos-history_size)+index) & (ringbuffer_size-1);
    return history[i];
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

} // namespace YAKC
