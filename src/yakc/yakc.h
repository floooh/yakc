#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::yakc
    @brief main emulator class
*/
#include "yakc/core.h"
#include "yakc/breadboard.h"
#include "yakc/kc85.h"
#include "yakc/z1013.h"
#include "yakc/z9001.h"

namespace YAKC {

class yakc {
public:
    device model = device::none;
    os_rom os = os_rom::none;
    breadboard board;
    class kc85 kc85;
    class z1013 z1013;
    class z9001 z9001;

    /// one-time init
    void init(const ext_funcs& funcs);
    /// poweron one of the emus
    void poweron(device m, os_rom os);
    /// poweroff the emu
    void poweroff();
    /// reset the emu
    void reset();
    /// check if currently emulated device matches
    bool is_device(device mask) const;
    /// process one frame, up to absolute number of cycles
    void onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count);
    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// get human-readable info about current system
    const char* system_info() const;
    /// get current border color
    void border_color(float& out_red, float& out_green, float& out_blue);
};

} // namespace YAKC
