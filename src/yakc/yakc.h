#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::yakc
    @brief main emulator class
*/
#include "yakc/util/rom_images.h"
#include "yakc/util/core.h"
#include "yakc/util/filesystem.h"
#include "yakc/util/tapedeck.h"
#include <functional>

namespace YAKC {

class yakc {
public:
    /// one-time init
    void init(const ext_funcs& funcs);
    /// add a ROM image
    void add_rom(rom_images::rom type, const uint8_t* ptr, int size);
    /// check if the required ROM images for a model/os combination are loaded
    bool check_roms(system model, os_rom os=os_rom::none);
    /// poweron one of the emus
    void poweron(system m, os_rom os=os_rom::none);
    /// poweroff the emu
    void poweroff();
    /// reset the emu
    void reset();
    /// process one frame, up to absolute number of cycles
    void exec(int micro_secs, uint64_t audio_cycle_count);
    /// step over one instruction and return number of cycles (called by debuggers)
    uint32_t step();
    /// step until function returns true
    uint32_t step_until(std::function<bool(uint32_t)> fn);

    /// called when an ASCII key is pressed
    void on_ascii(uint8_t ascii);
    /// called when a non-ascii key is pressed down
    void on_key_down(uint8_t keycode);
    /// called when a non-ascii key is released
    void on_key_up(uint8_t keycode);
    /// called per frame with new joystick input
    void on_joystick(uint8_t joy0_kbd_mask, uint8_t joy0_pad_mask);
    /// get number of supported joysticks of current system
    int num_joysticks() const;
    /// enable/disable joystick
    void enable_joystick(bool b);
    /// return true if joystick is enabled
    bool is_joystick_enabled() const;

    /// start a quickload (may not be finished when function returns)
    bool quickload(const char* name, filetype type, bool start);

    /// fill sample buffer for external audio system (may be called from a thread!)
    void fill_sound_samples(float* buffer, int num_samples);
    /// get pointer to emulator framebuffer, its width, and height
    const void* framebuffer(int& out_width, int& out_height);

    /// return true if switched on
    bool switchedon() const;
    /// check if currently emulated system matches
    bool is_system(system mask) const;
    /// check any model against system mask
    static bool is_system(system model, system mask);
    /// get the cpu model of the current system
    cpu_model cpu_type() const;
    /// get a bit mask of chip::id bits in the current system
    chip::mask chip_types() const;
    /// get human-readable info about current system
    const char* system_info() const;

    system model = system::none;
    os_rom os = os_rom::none;
    class filesystem filesystem;
    bool cpu_ahead = false;                 // cpu would have been ahead of max_cycle_count
    bool cpu_behind = false;                // cpu would have been behind of min_cycle_count
    uint64_t abs_cycle_count = 0;           // total CPU cycle count
    uint32_t overflow_cycles = 0;           // cycles that have overflowed from last frame
private:
    bool joystick_enabled = false;
};

} // namespace YAKC
