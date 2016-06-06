#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z9001
    @brief wrapper class for Z9001, KC85/1 and KC87 
*/
#include "yakc/breadboard.h"
#include "yakc/roms.h"

namespace YAKC {

class z9001 {
public:
    /// system RAM
    ubyte ram[4*0x4000];
    /// color RAM (40x24 color attributes)
    ubyte color_ram[0x400];
    /// video RAM (40x24 characters)
    ubyte video_ram[0x400];

    /// the main board
    breadboard* board = nullptr;

    /// one-time setup
    void init(breadboard* board);
    
    /// power-on the device
    void poweron(device m, os_rom os);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get the Z1013 model
    device model() const;
    /// get info about emulated system
    const char* system_info() const;

    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// process one frame, up to absolute number of cycles
    void onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count);

    /// the z80 out callback
    static void out_cb(void* userdata, uword port, ubyte val);
    /// the z80 in callback
    static ubyte in_cb(void* userdata, uword port);
    /// blink counter callback
    static void blink_cb(void* userdata);

    /// decode an entire frame into RGBA8Buffer
    void decode_video();

    device cur_model = device::kc87;
    os_rom cur_os = os_rom::kc87_os_2;
    bool on = false;
    bool cpu_ahead = false;
    bool cpu_behind = false;
    uint64_t abs_cycle_count = 0;
    uint32_t overflow_cycles = 0;

    bool blink_flipflop = false;
    uint32_t blink_counter = 0;
    uint32_t pal[8];
    uint32_t RGBA8Buffer[320*192];          // decoded linear RGBA8 video buffer
};

} // namespace YAKC
