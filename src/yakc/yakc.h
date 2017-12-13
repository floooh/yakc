#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::yakc
    @brief main emulator class
*/
#include "yakc/core/core.h"
#include "yakc/systems/breadboard.h"
#include "yakc/systems/rom_images.h"
#include "yakc/core/filesystem.h"
#include "yakc/peripherals/tapedeck.h"
#include "yakc/systems/z1013.h"
/*
#include "yakc/systems/kc85.h"
#include "yakc/systems/z9001.h"
#include "yakc/systems/zx.h"
#include "yakc/systems/cpc.h"
#include "yakc/systems/atom.h"
#include "yakc/systems/bbcmicro.h"
*/

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
    void step(int micro_secs, uint64_t audio_cycle_count);
    /// step over one instruction and return number of cycles (called by debuggers)
    uint32_t step_debug();

    /// put key and joystick input
    void put_input(uint8_t ascii, uint8_t joy0_kbd_mask, uint8_t joy0_pad_mask=0);
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

    /// do a partial init after applying a snapshot
    void on_context_switched();

    /// return true if switched on
    bool switchedon() const;
    /// check if currently emulated system matches
    bool is_system(system mask) const;
    /// check any model against system mask
    static bool is_system(system model, system mask);
    /// get the cpu model of the current system
    cpu_model cpu_type() const;
    /// get human-readable info about current system
    const char* system_info() const;
    /// get the currently active system_bus
    system_bus* get_bus();

    system model = system::none;
    os_rom os = os_rom::none;
    class z1013 z1013;
/*
    class kc85 kc85;
    class z9001 z9001;
    class zx zx;
    class cpc cpc;
    class atom atom;
    class bbcmicro bbcmicro;
*/
    breadboard board;
    class rom_images roms;
    class filesystem filesystem;
    class tapedeck tapedeck;

    bool cpu_ahead = false;                 // cpu would have been ahead of max_cycle_count
    bool cpu_behind = false;                // cpu would have been behind of min_cycle_count
    uint64_t abs_cycle_count = 0;           // total CPU cycle count
    uint32_t overflow_cycles = 0;           // cycles that have overflowed from last frame

private:
    bool joystick_enabled = false;
};

} // namespace YAKC
