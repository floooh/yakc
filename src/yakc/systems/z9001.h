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

namespace YAKC {

class z9001_t {
public:
    /// check if required roms are loaded
    static bool check_roms(system model, os_rom os);

    /// power-on the device
    void poweron(system m, os_rom os);
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
    /// the Z80 PIO1 out callback
    static void pio1_out(int port_id, uint8_t data, void* user_data);
    /// the Z80 PIO1 in callback
    static uint8_t pio1_in(int port_id, void* user_data);
    /// the Z80 PIO2 out callback
    static void pio2_out(int port_id, uint8_t data, void* user_data);
    /// the Z80 PIO2 in callback
    static uint8_t pio2_in(int port_id, void* user_data);

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

    /// initialize the memory map
    void init_memorymap();
    /// initialize the keyboard matrix
    void init_keymap();
    /// decode an entire frame into RGBA8Buffer
    void decode_video();

    static const int video_ram_page = 4;
    static const int color_ram_page = 5;

    system cur_model = system::kc87;
    os_rom cur_os = os_rom::kc87_os_2;
    bool on = false;

    uint64_t ctc_zcto2 = 0;     // pin mask to store state of CTC ZCTO2
    uint32_t blink_counter = 0;
    bool blink_flip_flop = false;
    uint8_t brd_color = 0;              // border color byte extracted from PIO1-A

    static const int display_width = 320;
    static const int display_height = 192;
    static_assert(display_width <= global_max_fb_width, "z9001 fb size");
    static_assert(display_height <= global_max_fb_height, "z9001 fb size");
};
extern class z9001_t z9001;

} // namespace YAKC
