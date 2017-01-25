#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::yakc
    @brief main emulator class
*/
#include "yakc/core/core.h"
#include "yakc/systems/breadboard.h"
#include "yakc/systems/rom_images.h"
#include "yakc/systems/kc85.h"
#include "yakc/systems/z1013.h"
#include "yakc/systems/z9001.h"
#include "yakc/systems/zx.h"
#include "yakc/systems/cpc.h"
#include "yakc/systems/atom.h"
#include "yakc/systems/bbcmicro.h"

namespace YAKC {

class yakc {
public:
    /// one-time init
    void init(const ext_funcs& funcs);
    /// add a ROM image
    void add_rom(rom_images::rom type, const uint8_t* ptr, int size);
    /// check if the required ROM images for a model/os combination are loaded
    bool check_roms(device model, os_rom os=os_rom::none);
    /// poweron one of the emus
    void poweron(device m, os_rom os=os_rom::none);
    /// poweroff the emu
    void poweroff();
    /// reset the emu
    void reset();
    /// process one frame, up to absolute number of cycles
    void step(int micro_secs, uint64_t audio_cycle_count);

    /// put key and joystick input
    void put_input(uint8_t ascii, uint8_t joy0_mask);
    /// enable/disable joystick
    void enable_joystick(bool b);
    /// return true if joystick is enabled
    bool is_joystick_enabled() const;

    /// fill sample buffer for external audio system (may be called from a thread!)
    void fill_sound_samples(float* buffer, int num_samples);
    /// get pointer to emulator framebuffer, its width, and height
    const void* framebuffer(int& out_width, int& out_height);

    /// clear the current interrupt daisychain
    void clear_daisychain();
    /// do a partial init after applying a snapshot
    void on_context_switched();

    /// return true if switched on
    bool switchedon() const;
    /// check if currently emulated device matches
    bool is_device(device mask) const;
    /// check any model against device mask
    static bool is_device(device model, device mask);
    /// get the cpu model of the current device
    cpu_model cpu_type() const;
    /// get human-readable info about current system
    const char* system_info() const;
    /// get current border color
    void border_color(float& out_red, float& out_green, float& out_blue);
    /// get the currently active system_bus
    system_bus* get_bus();

    device model = device::none;
    os_rom os = os_rom::none;
    class kc85 kc85;
    class z1013 z1013;
    class z9001 z9001;
    class zx zx;
    class cpc cpc;
    class atom atom;
    class bbcmicro bbcmicro;
    breadboard board;
    class rom_images roms;

    bool cpu_ahead = false;                 // cpu would have been ahead of max_cycle_count
    bool cpu_behind = false;                // cpu would have been behind of min_cycle_count
    uint64_t abs_cycle_count = 0;           // total CPU cycle count
    uint32_t overflow_cycles = 0;           // cycles that have overflowed from last frame

private:
    bool joystick_enabled = false;
};

} // namespace YAKC
