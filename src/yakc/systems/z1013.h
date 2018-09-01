#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z1013
    @brief wrapper class for Z1013 system
    
    http://hc-ddr.hucki.net/wiki/doku.php/z1013:hardware
    http://hc-ddr.hucki.net/wiki/doku.php/z1013:software:monitor:riesa202
    http://www.z1013.de/hardware-uebersicht.html    
    
    On keyboard input:
    This is the only tricky part of the emulation. The original Z1013
    has a 8x4 keyboard matrix with 4 shift keys. The ROM will activate
    every column of the keyboard matrix by outputting the column
    number on port 8, and expects the active line bits of the
    keyboard matrix in the low 4 bits of PIO-B.
    
    The Z1013.16 and Z1013.64 with ROM A2 support a 8x8 keyboard
    matrix, but still only reads 4 bits from PIO-B to get the matrix
    line state (but needs 8). Writing bit 4 of PIO-B is used to
    select whether the lower 4 or higher 4 bit of the 8 keyboard 
    matrix lines is selected. A set bit 4 in PIO-B means that
    the high-4-bits are requested, otherwise the low-4-bits.

    For the 8x8 keyboard matrix layout, see here: 
    http://www.z1013.de/images/21.gif
    
    The keyboard matrix state for each ASCII key is encoded in a
    lookup table with 64-bit entries (of which only 32 bits are
    used for the old 8x4 keyboard matrix). For each ASCII code,
    the associated keyboard matrix state (either 32 bit for the 8x4
    or 64 bit for the 8x8 keyboard) is stored in the lookup table
    'key_map'. The full bit mask is necessary because an ASCII code
    can require more than one key to be set (e.g. shift keys).
*/
#include "yakc/util/breadboard.h"
#include "yakc/util/rom_images.h"
#include "yakc/util/filesystem.h"
#include "yakc/util/filetypes.h"
#include "systems/z1013.h"

namespace YAKC {

class z1013_t {
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
    /// get framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// file quickloading
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);
    /// audio callback
    static void audio_cb(const float* samples, int num_samples, void* user_data);

    ::z1013_t sys;
    system cur_model = system::none;
    bool on = false;
};
extern YAKC::z1013_t z1013;

} // namespace YAKC
