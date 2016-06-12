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
    this->z9001.init(&this->board);
}

//------------------------------------------------------------------------------
void
yakc::poweron(device m, os_rom rom) {
    YAKC_ASSERT(!this->kc85.on && !this->z1013.on);
    this->model = m;
    this->os = rom;
    if (this->is_device(device::any_kc85)) {
        this->kc85.poweron(m, rom);
    }
    else if (this->is_device(device::any_z1013)) {
        this->z1013.poweron(m);
    }
    else if (this->is_device(device::any_z9001)) {
        this->z9001.poweron(m, rom);
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
    if (this->z9001.on) {
        this->z9001.poweroff();
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
    if (this->z9001.on) {
        this->z9001.reset();
    }
}

//------------------------------------------------------------------------------
bool
yakc::is_device(device mask) const {
    return 0 != (int(this->model) & int(mask));
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
    if (this->z9001.on) {
        this->z9001.onframe(speed_multiplier, micro_secs, min_cycle_count, max_cycle_count);
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
    if (this->z9001.on) {
        this->z9001.put_key(ascii);
    }
}

//------------------------------------------------------------------------------
const char*
yakc::system_info() const {
    if (this->kc85.on) {
        return this->kc85.system_info();
    }
    else if (this->z1013.on) {
        return this->z1013.system_info();
    }
    else if (this->z9001.on) {
        return this->z9001.system_info();
    }
    else {
        return "no info available";
    }
}

//------------------------------------------------------------------------------
void
yakc::border_color(float& out_red, float& out_green, float& out_blue) {
    if (this->z9001.on) {
        this->z9001.border_color(out_red, out_green, out_blue);
    }
    else {
        out_red = out_green = out_blue = 0.0f;
    }
}

} // namespace YAKC
