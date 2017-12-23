#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::zx
    @brief Sinclair ZX Spectrum 48K/128K emulation
*/
#include "yakc/systems/breadboard.h"
#include "yakc/systems/rom_images.h"
#include "yakc/core/filesystem.h"
#include "yakc/core/filetypes.h"

namespace YAKC {

class zx_t {
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

    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);
    /// perform a single debug-step
    uint32_t step_debug();
    /// the Z80 CPU tick callback
    static uint64_t cpu_tick(int num_ticks, uint64_t pins);

    /// put a key and joystick input (Kempston)
    void put_input(uint8_t ascii, uint8_t joy0_mask);
    /// decode audio data
    void decode_audio(float* buffer, int num_samples);
    /// get framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// file quickloading
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);    

    /// initialize the memory map
    void init_memorymap();
    /// initialize the keyboard matrix mapping table
    void init_keymap();
    /// decode the next line into RGBA8Buffer
    void decode_video_line(uint16_t y);
    /// called by timer for each PAL scanline (decodes 1 line of vidmem), return true if vblank interrupt
    bool scanline();

    static uint32_t palette[8];

    system cur_model = system::zxspectrum48k;
    bool on = false;
    bool memory_paging_disabled = false;
    bool int_requested = false;
    uint8_t last_fe_out = 0;            // last OUT value to xxFE port
    uint8_t blink_counter = 0;          // increased by one every vblank
    int scanline_period = 0;
    int scanline_counter = 0;           // tick counter for one scanline
    int scanline_y = 0;                 // current scanline-y
    int frame_scanlines = 0;            // number of scanlines in a frame
    int top_border_scanlines = 0;       // height of top border
    uint32_t display_ram_bank = 0;      // which RAM bank to use as display mem
    uint32_t border_color = 0xFF000000;

    static const int display_width = 320;
    static const int display_height = 256;

    uint8_t joy_mask = 0;                 // joystick mask
};
extern zx_t zx;

} // namespace YAKC
