#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::c64
    @brief Commodore C64 emulation
*/
#include "yakc/util/breadboard.h"
#include "yakc/util/rom_images.h"
#include "yakc/util/filetypes.h"
#include "yakc/util/filesystem.h"
#include "systems/c64.h"

namespace YAKC {

class c64_t {
public:
    /// check if required roms are loaded
    static bool check_roms(system model);

    /// power-on the device
    void poweron(system model);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get info about emulated system
    const char* system_info() const;
    /// return number of supported joysticks
    int num_joysticks() const { return 1; };
    /// run the emulation
    void exec(uint32_t micro_seconds);
    /// called when alpha-numeric key has been pressed
    void on_ascii(uint8_t ascii);
    /// called when non-alnum key has been pressed down
    void on_key_down(uint8_t key);
    /// called when non-alnum key has been released
    void on_key_up(uint8_t key);
    /// called for joystick input
    void on_joystick(uint8_t mask);
    /// get framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// decode audio data
    void decode_audio(float* buffer, int num_samples);
    /// called when a tape has been inserted into the tape deck
    void on_tape_inserted();
    /// file quickloading (only bin/raw files)
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);
    /// audio callback
    static void audio_cb(const float* samples, int num_samples, void* user_data);

    ::c64_t sys;
    bool on = false;
};
extern YAKC::c64_t c64;

} // namespace YAKC

