#pragma once
//------------------------------------------------------------------------------
/** 
    @class YAKC::atom
    @brief Acorn Atom emulation
    
    https://fjkraan.home.xs4all.nl/comp/atom/index.html
*/
#include "yakc/util/breadboard.h"
#include "yakc/util/rom_images.h"
#include "yakc/util/filesystem.h"
#include "yakc/util/filetypes.h"
#include "systems/atom.h"

namespace YAKC {

class atom_t {
public:
    /// check if required roms are loaded
    static bool check_roms(system model);

    /// power-on the device
    void poweron();
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
    /// file quickloading
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);
    /// audio callback 
    static void audio_cb(const float* samples, int num_samples, void* user_data);

    ::atom_t sys;
    bool on = false;
};
extern YAKC::atom_t atom;

} // namespace YAKC

