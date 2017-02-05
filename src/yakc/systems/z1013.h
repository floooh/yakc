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
#include "yakc/core/system_bus.h"
#include "yakc/systems/breadboard.h"
#include "yakc/systems/rom_images.h"
#include "yakc/systems/filesystem.h"

namespace YAKC {

class z1013 : public system_bus {
public:
    /// the main board
    breadboard* board = nullptr;
    /// rom image store
    rom_images* roms = nullptr;

    /// one-time setup
    void init(breadboard* board, rom_images* roms);
    /// check if required roms are loaded
    static bool check_roms(const rom_images& roms, system model, os_rom os);
    /// initialize memory mapping (called from poweron or snapshot restore)
    void init_memory_mapping();
    /// initialize keymap tables (called from poweron or snapshot restore)
    void init_keymaps();

    /// power-on the device
    void poweron(system m);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get info about emulated system
    const char* system_info() const;
    /// called after snapshot restore
    void on_context_switched();
    /// get framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// file quickloading
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);

    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);
    /// perform a single debug-step
    uint32_t step_debug();

    /// the z80 out callback
    virtual void cpu_out(uword port, ubyte val) override;
    /// the z80 in callback
    virtual ubyte cpu_in(uword port) override;
    /// PIO out callback
    virtual void pio_out(int pio_id, int port_id, ubyte val) override;
    /// PIO in callback
    virtual ubyte pio_in(int pio_id, int port_id) override;
    /// interrupt request callback
    virtual void irq(bool b) override;

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

    static const int vidmem_page = 4;
    system cur_model = system::z1013_01;
    os_rom cur_os = os_rom::z1013_mon202;
    bool on = false;
    ubyte kbd_column_nr_requested = 0;      // requested keyboard matrix column number (0..7)
    bool kbd_8x8_requested = false;         // bit 4 in PIO-B written
    uint64_t next_kbd_column_bits = 0;
    uint64_t kbd_column_bits = 0;
    static const int max_num_keys = 128;
    uint64_t key_map[max_num_keys] = { };   // map ASCII code to keyboard matrix bits

    static const int display_width = 256;
    static const int display_height = 256;
    static_assert(display_width <= global_max_fb_width, "z1013 fb size");
    static_assert(display_height <= global_max_fb_height, "z1013 fb size");
    uint32_t* rgba8_buffer = nullptr;
};

} // namespace YAKC
