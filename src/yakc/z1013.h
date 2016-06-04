#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z1013
    @brief wrapper class for Z1013 system
    
    http://hc-ddr.hucki.net/wiki/doku.php/z1013:hardware
    http://hc-ddr.hucki.net/wiki/doku.php/z1013:software:monitor:riesa202
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

    /// initialize the key translation table
    void init_key_map();
    /// add a single key to the key map with the keyboard matrix column/line and shift key (0..4)
    void init_key(ubyte ascii, int col, int line, int shift=0);
    /// get keyboard matrix bit mask by column and line
    uint32_t kbd_bit(int col, int line);
    /// get keyboard matrix column bits
    ubyte get_kbd_column_bits(int col) const;

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
    ubyte kbd_column_nr_requested = 0;
    uint32_t next_kbd_column_bits = 0;
    uint32_t kbd_column_bits = 0;
    static const int max_num_keys = 128;
    uint32_t key_map[max_num_keys] = { };   // map ASCII code to keyboard matrix bits

    uint32_t RGBA8Buffer[256*256];          // decoded linear RGBA8 video buffer
};

} // namespace YAKC
