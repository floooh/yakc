//------------------------------------------------------------------------------
//  yakc.cc
//------------------------------------------------------------------------------
#include "yakc.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
yakc::init(const ext_funcs& funcs) {
    func = funcs;
    this->kc85.init(&this->board);
    this->z1013.init(&this->board);
}

//------------------------------------------------------------------------------
void
yakc::poweron(device m, os_rom rom) {
    YAKC_ASSERT(!this->kc85.on && !this->z1013.on);
    this->model = m;
    this->os = rom;
    if (int(m) & int(device::any_kc85)) {
        this->kc85.poweron(m, rom);
    }
    else if (int(m) & int(device::any_z1013)) {
        this->z1013.poweron(m);
    }
}

//------------------------------------------------------------------------------
void
yakc::poweroff() {
    if (this->kc85.on) {
        this->kc85.poweroff();
    }
    if (this->z1013.on) {
        this->z1013.poweroff();
    }
}

//------------------------------------------------------------------------------
void
yakc::reset() {
    if (this->kc85.on) {
        this->kc85.reset();
    }
    if (this->z1013.on) {
        this->z1013.reset();
    }
}

//------------------------------------------------------------------------------
bool
yakc::is_device(device mask) {
    return (int(this->model) & int(mask));
}

//------------------------------------------------------------------------------
void
yakc::onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count) {
    if (this->kc85.on) {
        this->kc85.onframe(speed_multiplier, micro_secs, min_cycle_count, max_cycle_count);
    }
    if (this->z1013.on) {
        this->z1013.onframe(speed_multiplier, micro_secs, min_cycle_count, max_cycle_count);
    }
}

//------------------------------------------------------------------------------
void
yakc::put_key(ubyte ascii) {
    if (this->kc85.on) {
        this->kc85.put_key(ascii);
    }
    if (this->z1013.on) {
        this->z1013.put_key(ascii);
    }
}

} // namespace YAKC
