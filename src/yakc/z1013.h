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
#include "yakc/breadboard.h"
#include "yakc/roms.h"
#include "yakc/z1013_roms.h"

namespace YAKC {

class z1013 {
public:
    /// ram banks
    ubyte ram[4][0x4000];
    /// 1 Kbyte separate video memory
    ubyte irm[0x400];

    /// hardware components
    breadboard* board = nullptr;
    z1013_roms roms;

    /// one-time setup
    void init(breadboard* board);

    /// power-on the device
    void poweron(device m);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get the Z1013 model
    device model() const;
    /// get info about emulated system
    const char* system_info() const;

    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// process one frame, up to absolute number of cycles
    void onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count);

    /// the z80 out callback
    static void out_cb(void* userdata, uword port, ubyte val);
    /// the z80 in callback
    static ubyte in_cb(void* userdata, uword port);
    /// PIO-A out callback
    static void pio_a_out_cb(void* userdata, ubyte val);
    /// PIO-A in callback
    static ubyte pio_a_in_cb(void* userdata);
    /// PIO-B out callback
    static void pio_b_out_cb(void* userdata, ubyte val);
    /// PIO-B in callback
    static ubyte pio_b_in_cb(void* userdata);

    /// initialize the key translation table for the basic 8x4 keyboard (z1013.01)
    void init_keymap_8x4();
    /// initialize the key translation table for the 8x8 keyboard (z1013.16/64)
    void init_keymap_8x8();
    /// add a single key to the key map with the keyboard matrix column/line and shift key (0..4)
    void init_key(ubyte ascii, int col, int line, int shift, int num_lines);
    /// get keyboard matrix bit mask by column and line
    uint64_t kbd_bit(int col, int line, int num_lines);

    /// decode an entire frame into RGBA8Buffer
    void decode_video();

    device cur_model = device::z1013_01;
    os_rom cur_os = os_rom::z1013_mon202;
    bool on = false;
    bool cpu_ahead = false;
    bool cpu_behind = false;
    uint64_t abs_cycle_count = 0;
    uint32_t overflow_cycles = 0;
    const ubyte* os_ptr = nullptr;
    int os_size = 0;
    ubyte kbd_column_nr_requested = 0;      // requested keyboard matrix column number (0..7)
    bool kbd_8x8_requested = false;         // bit 4 in PIO-B written
    uint64_t next_kbd_column_bits = 0;
    uint64_t kbd_column_bits = 0;
    static const int max_num_keys = 128;
    uint64_t key_map[max_num_keys] = { };   // map ASCII code to keyboard matrix bits

    uint32_t RGBA8Buffer[256*256];          // decoded linear RGBA8 video buffer
};

} // namespace YAKC
