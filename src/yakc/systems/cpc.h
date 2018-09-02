#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::cpc
    @brief Amstrad CPC 464/6128 and KC Compact emulation
*/
#include "yakc/util/breadboard.h"
#include "yakc/util/rom_images.h"
#include "yakc/util/filesystem.h"
#include "yakc/util/tapedeck.h"
#include "systems/cpc.h"

namespace YAKC {

class cpc_t {
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
    /// decode next audio buffer
    void decode_audio(float* buffer, int num_samples);
    /// get pointer to framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// file quickloading
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);
    /// audio callback
    static void audio_cb(const float* samples, int num_samples, void* user_data);
    /// video debugging callback
    static void video_debug_cb(uint64_t crtc_pins, void* user_data);

    system cur_model = system::cpc464;
    bool on = false;
    ::cpc_t sys;
    static const int dbg_width = 1024;
    static const int dbg_height = 312;
};
extern YAKC::cpc_t cpc;

} // namespace YAKC
