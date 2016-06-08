#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::z9001
    @brief wrapper class for Z9001, KC85/1 and KC87
    
    KEYBOARD MATRIX:
        PIO2-A is connected to matrix columns (active low)
        PIO2-B is connected to matrix lines (ditto)
        
        Simple returning the ~column on PIO2-A and ~line on PIO2-B
        will start outputting that character to the console.
        
          | 0   1   2   3   4   5   6   7
        --+------------------------------
        0 | 0   1   2   3   4   5   6   7
          |
        1 | 8   9   :   ;   ,   =   .   ?
          |
        2 | @   A   B   C   D   E   F   G
          |
        3 | H   I   J   K   L   M   N   O
          |
        4 | P   Q   R   S   T   U   V   W
          |
        5 | X   Y   Z  $19 $13 $1A  ^  $7F
          |
        6 |$08 $09 $0A $0B $1B $0D $03?$20
          |
        7 |[S] $14 $7F $00 $1C $1D $00 $AB

        [S]     shift key(?)
        $03:    stop
        $08:    cursor left
        $09:    cursor right
        $0A:    cursor up
        $0B:    cursor down
        $0D:    enter
        $13:    pause
        $14:    color
        $19:    home
        $1A:    insert
        $1B:    esc
        $1C:    list
        $1D:    run
*/
#include "yakc/breadboard.h"
#include "yakc/roms.h"

namespace YAKC {

class z9001 {
public:
    /// system RAM
    ubyte ram[4*0x4000];
    /// color RAM (40x24 color attributes)
    ubyte color_ram[0x400];
    /// video RAM (40x24 characters)
    ubyte video_ram[0x400];

    /// the main board
    breadboard* board = nullptr;

    /// one-time setup
    void init(breadboard* board);
    
    /// power-on the device
    void poweron(device m, os_rom os);
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
    /// PIO2-A OUT callback (triggers keyboard matrix columns)
    static void pio2_a_out_cb(void* userdata, ubyte val);
    /// PIO2-A OUT callback (triggers keyboard matrix lines
    static void pio2_b_out_cb(void* userdata, ubyte val);
    /// PIO2-A IN callback for keyboard input (keyboard matrix column)
    static ubyte pio2_a_in_cb(void* userdata);
    /// PIO2-B IN callback for keyboard input (keyboard matrix line)
    static ubyte pio2_b_in_cb(void* userdata);
    /// blink counter callback
    static void blink_cb(void* userdata);

    /// decode an entire frame into RGBA8Buffer
    void decode_video();

    device cur_model = device::kc87;
    os_rom cur_os = os_rom::kc87_os_2;
    bool on = false;
    bool cpu_ahead = false;
    bool cpu_behind = false;
    uint64_t abs_cycle_count = 0;
    uint32_t overflow_cycles = 0;

    ubyte kbd_column_mask = 0;
    ubyte kbd_line_mask = 0;

    bool blink_flipflop = false;
    uint32_t blink_counter = 0;
    uint32_t pal[8];
    uint32_t RGBA8Buffer[320*192];          // decoded linear RGBA8 video buffer
};

} // namespace YAKC
