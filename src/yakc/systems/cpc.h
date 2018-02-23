#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::cpc
    @brief Amstrad CPC 464/6128 and KC Compact emulation
*/
#include "yakc/util/breadboard.h"
#include "yakc/util/rom_images.h"
#include "yakc/util/filesystem.h"
#include "yakc/util/tapedeck.h"

namespace YAKC {

class cpc_t {
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
    int num_joysticks() const { return 1; };
    /// process a number of cycles, return final processed tick
    uint64_t exec(uint64_t start_tick, uint64_t end_tick);

    /// the Z80 CPU tick callback
    static uint64_t cpu_tick(int num_ticks, uint64_t pins);
    /// perform a CPU IO request (called from cpu_tick)
    static uint64_t cpu_iorq(uint64_t pins);
    /// i8255 output callback
    static uint64_t ppi_out(int port_id, uint64_t pins, uint8_t data);
    /// i8255 input callback
    static uint8_t ppi_in(int port_id);
    /// AY-3-8912 port output callback
    static void psg_out(int port_id, uint8_t data);
    /// AY-3-8912 port input callback
    static uint8_t psg_in(int port_id);

    /// initialize the gate array (called from init and reset)
    void ga_init();
    /// tick the gate array
    uint64_t ga_tick(uint64_t pins);
    /// called on an interrupt acknowledge machine cycle
    void ga_int_ack();
    /// called when the interrupt-control bit is set by a CPU IO operation
    void ga_int_ctrl();
    /// top-level video frame decoding
    void ga_decode_video(uint64_t crtc_pins);
    /// decode the next pixel block
    void ga_decode_pixels(uint32_t* dst, uint64_t crtc_pins);

    /// update bank switching
    void update_memory_mapping();
    /// initialize the keycode translation map
    void init_keymap();

    /// called when alpha-numeric key has been pressed
    void on_ascii(uint8_t ascii);
    /// called when non-alnum key has been pressed down
    void on_key_down(uint8_t key);
    /// called when non-alnum key has been released
    void on_key_up(uint8_t key);
    /// called for joystick input
    void on_joystick(uint8_t mask);
    /// decode next audio buffer
    void decode_audio(float* buffer, int num_samples);
    /// get pointer to framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// file quickloading
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);
    /// load an SNA snapshot file
    bool load_sna(filesystem* fs, const char* name, filetype type, bool start);
    /// load a AMSDOS header bin file
    bool load_bin(filesystem* fs, const char* name, filetype type, bool start);
    /// the trapped casread function for TAP files
    void casread();

    system cur_model = system::cpc464;
    bool on = false;
    uint32_t tick_count = 0;
    uint16_t casread_trap = 0x0000;
    uint16_t casread_ret = 0x0000;
    uint8_t joymask = 0;

    uint8_t ga_config = 0;          // out to port 0x7Fxx func 0x80
    uint8_t ga_next_video_mode = 0;
    uint8_t ga_video_mode = 0;
    uint8_t ga_ram_config = 0;      // out to port 0x7Fxx func 0xC0
    uint8_t ga_pen = 0x00;          // currently selected pen (or border)
    uint32_t ga_colors[32];         // CPC and KC Compact have different colors
    uint32_t ga_palette[16];        // the current pen colors
    uint32_t ga_border_color = 0;   // the current border color
    int ga_hsync_irq_counter = 0;   // incremented each scanline, reset at 52
    int ga_hsync_after_vsync_counter = 0;   // for 2-hsync-delay after vsync
    int ga_hsync_delay_counter = 0; // hsync to monitor is delayed 2 ticks
    int ga_hsync_counter = 0;       // countdown until hsync to monitor is deactivated
    bool ga_sync = false;           // gate-array generated video sync (modified HSYNC)
    bool ga_int = false;            // GA interrupt pin active
    uint64_t ga_crtc_pins;          // store CRTC pins to detect rising/falling bits
    
    bool debug_video = false;
    static const int max_display_width = 768;
    static const int max_display_height = 272;
    static_assert(max_display_width <= global_max_fb_width, "cpc display size");
    static_assert(max_display_height <= global_max_fb_height, "cpc display size");
    static const int dbg_max_display_width  = 1024;     // 64*16
    static const int dbg_max_display_height = 312;
    static_assert(dbg_max_display_width <= global_max_fb_width, "cpc display size");
    static_assert(dbg_max_display_height <= global_max_fb_height, "cpc display size");
};
extern cpc_t cpc;

} // namespace YAKC
