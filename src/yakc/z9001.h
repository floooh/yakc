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

    /// put a key as ASCII code
    void put_key(ubyte ascii);

    /// decode an entire frame into RGBA8Buffer
    void decode_video();

    device cur_model = device::kc87;
    os_rom cur_os = os_rom::kc87_os_2;
    bool on = false;
    bool cpu_ahead = false;
    bool cpu_behind = false;
    uint64_t abs_cycle_count = 0;
    uint32_t overflow_cycles = 0;

    uint64_t key_mask = 0;              // (column<<8)|line bits for currently pressed key
    uint8_t kbd_column_mask = 0;        // PIO2-A keyboard matrix column mask
    uint8_t kbd_line_mask = 0;          // PIO2-B keyboard matrix line mask
    static const int max_num_keys = 128;
    uint64_t key_map[max_num_keys];     // complete keyboard matrix state for each ascii code

    bool blink_flipflop = false;
    uint32_t blink_counter = 0;
    uint32_t pal[8];
    uint32_t RGBA8Buffer[320*192];          // decoded linear RGBA8 video buffer
};

} // namespace YAKC
