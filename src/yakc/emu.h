#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::emu
    @brief simple wrapper class which bundles all the emulators
*/
#include "yakc/common.h"
#include "yakc/breadboard.h"
#include "yakc/kc85.h"
#include "yakc/z1013.h"

namespace yakc {

class emu {
public:
    device model = device::none;
    os_rom os = os_rom::none;
    breadboard board;
    class kc85 kc85;
    class z1013 z1013;

    /// one-time init
    void init();
    /// poweron one of the emus
    void poweron(device m, os_rom os);
    /// poweroff the emu
    void poweroff();
    /// reset the emu
    void reset();
    /// process one frame, up to absolute number of cycles
    void onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count);
    /// put a key as ASCII code
    void put_key(ubyte ascii);
};

//------------------------------------------------------------------------------
inline void
emu::init() {
    this->kc85.setup(&this->board);
    this->z1013.setup(&this->board);
}

//------------------------------------------------------------------------------
inline void
emu::poweron(device m, os_rom rom) {
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
inline void
emu::poweroff() {
    if (this->kc85.on) {
        this->kc85.poweroff();
    }
    if (this->z1013.on) {
        this->z1013.poweroff();
    }
}

//------------------------------------------------------------------------------
inline void
emu::reset() {
    if (this->kc85.on) {
        this->kc85.reset();
    }
    if (this->z1013.on) {
        this->z1013.reset();
    }
}

//------------------------------------------------------------------------------
inline void
emu::onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count) {
    if (this->kc85.on) {
        this->kc85.onframe(speed_multiplier, micro_secs, min_cycle_count, max_cycle_count);
    }
    if (this->z1013.on) {
        this->z1013.onframe(speed_multiplier, micro_secs, min_cycle_count, max_cycle_count);
    }
}

//------------------------------------------------------------------------------
inline void
emu::put_key(ubyte ascii) {
    if (this->kc85.on) {
        this->kc85.put_key(ascii);
    }
    if (this->z1013.on) {
        this->z1013.put_key(ascii);
    }
}

} // namespace yakc
