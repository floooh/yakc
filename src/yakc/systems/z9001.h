#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z9001
    @brief wrapper class for Z9001, KC85/1 and KC87
    
    KEYBOARD MATRIX:
        PIO2-A => kbd matrix columns
        PIO2-B => kbd matrix lines

        Writing to PIO2-B to trigger an interrupt and then returning the 
        ~column on PIO2-A and ~line on PIO2-B will start outputting that 
        character to the console.
        
        http://www.sax.de/~zander/z9001/z9sch_5.pdf
        http://www.sax.de/~zander/z9001/z9sch_1.pdf
*/
#include "yakc/util/breadboard.h"
#include "yakc/util/rom_images.h"
#include "yakc/util/filesystem.h"
#include "yakc/util/filetypes.h"
#include "systems/z9001.h"

namespace YAKC {

class z9001_t {
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
    int num_joysticks() const { return 0; };
    /// run the emulation
    void exec(uint32_t micro_seconds);
    /// called when alpha-numeric key has been pressed
    void on_ascii(uint8_t ascii);
    /// called when non-alnum key has been pressed down
    void on_key_down(uint8_t key);
    /// called when non-alnum key has been released
    void on_key_up(uint8_t key);
    /// decode audio data
    void decode_audio(float* buffer, int num_samples);
    /// get framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// file quickloading
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);
    /// audio callback
    static void audio_cb(const float* samples, int num_samples, void* user_data);

    system cur_model = system::kc87;
    bool on = false;
    ::z9001_t sys;
};
extern class YAKC::z9001_t z9001;

} // namespace YAKC
