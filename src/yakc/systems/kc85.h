#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::kc85
    @brief wrapper class for the KC85/2, /3, /4
*/
#include "yakc/systems/breadboard.h"
#include "yakc/systems/rom_images.h"
#include "yakc/core/filesystem.h"
#include "yakc/core/filetypes.h"
#include "yakc/systems/kc85_exp.h"

namespace YAKC {

class kc85_t {
public:
    /// IO bits
    enum {
        PIO_A_CAOS_ROM    = (1<<0),
        PIO_A_RAM         = (1<<1),
        PIO_A_IRM         = (1<<2),
        PIO_A_RAM_RO      = (1<<3),
        PIO_A_UNUSED      = (1<<4),
        PIO_A_TAPE_LED    = (1<<5),
        PIO_A_TAPE_MOTOR  = (1<<6),
        PIO_A_BASIC_ROM   = (1<<7),

        PIO_B_VOLUME_MASK = (1<<5)-1,
        PIO_B_RAM8 = (1<<5),            // KC85/4 only
        PIO_B_RAM8_RO = (1<<6),         // KC85/4 only
        PIO_B_BLINK_ENABLED = (1<<7),

        // KC85/4 only
        IO84_SEL_VIEW_IMG   = (1<<0),  // 0: display img0, 1: display img1
        IO84_SEL_CPU_COLOR  = (1<<1),  // 0: access pixels, 1: access colors
        IO84_SEL_CPU_IMG    = (1<<2),  // 0: access img0, 1: access img1
        IO84_HICOLOR        = (1<<3),  // 0: hicolor mode off, 1: hicolor mode on
        IO84_SEL_RAM8       = (1<<4),  // select RAM8 block 0 or 1
        IO84_BLOCKSEL_RAM8  = (1<<5),  // no idea what that does...?

        IO86_RAM4   = (1<<0),
        IO86_RAM4_RO = (1<<1),
        IO86_CAOS_ROM_C = (1<<7)
    };

    kc85_exp exp;
    uint8_t pio_a = 0;          // current pio_a content, used by bankswitching
    uint8_t pio_b = 0;          // current pio_b content, used by bankswitching
    uint8_t io84 = 0;           // special KC85/4 io register
    uint8_t io86 = 0;           // special KC85/4 io register

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
    
    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);
    /// perform a single debug-step
    uint32_t step_debug();
    /// the CPU tick callback
    static uint64_t cpu_tick(int num_ticks, uint64_t pins);
    /// the PIO out callback
    static void pio_out(int port_id, uint8_t data);
    /// the PIO in callback
    static uint8_t pio_in(int port_id);

    /// decode audio data
    void decode_audio(float* buffer, int num_samples);
    /// decode 8 pixels
    void decode_8pixels(uint32_t* ptr, uint8_t pixels, uint8_t colors, bool blink_off) const;
    /// decode current video scanline
    void decode_scanline();
    /// get pointer to framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);

    /// put a key as ASCII code
    void put_key(uint8_t ascii);
    /// handle keyboard input
    void handle_keyboard_input();
    /// file quickloading
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);

    /// update module/memory mapping
    void update_bank_switching();
    /// update the rom pointers
    void update_rom_pointers();

    system cur_model = system::kc85_3;
    os_rom cur_caos = os_rom::caos_3_1;
    bool on = false;
    uint8_t key_code = 0;
    uint8_t* caos_c_ptr = nullptr;
    int caos_c_size = 0;
    uint8_t* caos_e_ptr = nullptr;
    int caos_e_size = 0;
    uint8_t* basic_ptr = nullptr;
    int basic_size = 0;

    // video hardware
    static const int display_width = 320;
    static const int display_height = 256;
    static_assert(display_width <= global_max_fb_width, "kc85 fb size");
    static_assert(display_height <= global_max_fb_height, "kc85 fb size");
    static const int irm0_page = 4;
    static const int num_scanlines = 312;
    int scanline_period = 0;
    int scanline_counter = 0;
    int cur_scanline = 0;
    bool ctc_blink_flag = true;
};
extern kc85_t kc85;

} // namespace YAKC

