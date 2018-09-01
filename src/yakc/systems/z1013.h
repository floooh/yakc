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

namespace YAKC {

class z1013_t {
public:
    /// check if required roms are loaded
    static bool check_roms(system model, os_rom os);

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
    /// process a number of cycles, return final processed tick
    uint64_t exec(uint64_t start_tick, uint64_t end_tick);

    /// the Z80 CPU tick callback
    static uint64_t cpu_tick(int num_ticks, uint64_t pins, void* user_data);
    /// the Z80 PIO out callback
    static void pio_out(int port_id, uint8_t data, void* user_data);
    /// the Z80 PIO in callback
    static uint8_t pio_in(int port_id, void* user_data);

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

    /// initialize the key translation table for the basic 8x4 keyboard (z1013.01)
    void init_keymap_8x4();
    /// initialize the key translation table for the 8x8 keyboard (z1013.16/64)
    void init_keymap_8x8();
    /// initialize memory mapping (called from poweron)
    void init_memorymap();
    /// initialize keymap tables (called from poweron)
    void init_keymaps();
    /// decode an entire frame into RGBA8Buffer
    void decode_video();

    z80_t z80;
    z80pio_t z80pio;
    kbd_t kbd;
    mem_t mem;

    static const int vidmem_page = 4;
    system cur_model = system::z1013_01;
    os_rom cur_os = os_rom::z1013_mon202;
    bool on = false;
    uint8_t kbd_request_column = 0;             // requested keyboard matrix column number (0..7)
    bool kbd_request_line_hilo = false;         // bit 4 in PIO-B written

    static const int display_width = 256;
    static const int display_height = 256;
    static_assert(display_width <= global_max_fb_width, "z1013 fb size");
    static_assert(display_height <= global_max_fb_height, "z1013 fb size");
};
extern class z1013_t z1013;

} // namespace YAKC
