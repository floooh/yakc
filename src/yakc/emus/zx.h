#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::zx
    @brief Sinclair ZX Spectrum 48K/128K emulation
*/
#include "yakc/util/breadboard.h"
#include "yakc/util/rom_images.h"
#include "yakc/util/filesystem.h"
#include "yakc/util/filetypes.h"
#include "systems/zx.h"

namespace YAKC {

class zx_t {
public:
    /// check if required roms are loaded
    static bool check_roms(system model);
    
    /// power-on the device
    void poweron(system m);
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
    /// decode audio data
    void decode_audio(float* buffer, int num_samples);
    /// get framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// file quickloading
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);    
    /// audio callback
    static void audio_cb(const float* samples, int num_samples, void* user_data);

    system cur_model = system::zxspectrum48k;
    bool on = false;
    ::zx_t sys;
};
extern YAKC::zx_t zx;

} // namespace YAKC
