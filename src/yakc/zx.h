#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::zx
    @brief Sinclair ZX Spectrum 48K/128K emulation
*/
#include "yakc/breadboard.h"
#include "yakc/rom_images.h"
#include "yakc/system_bus.h"
#include "yakc/sound_beeper.h"

namespace YAKC {

class zx : public system_bus {
public:
    /// the main board
    breadboard* board = nullptr;
    /// rom image storage
    rom_images* roms = nullptr;

    /// one-time setup
    void init(breadboard* board, rom_images* roms);
    /// check if required roms are loaded
    static bool check_roms(const rom_images& roms, device model, os_rom os);    
    /// initialize the memory map
    void init_memory_map();
    /// initialize the keyboard matrix mapping table
    void init_keymap();
    /// initialize a single entry in the key-map table
    void init_key_mask(ubyte ascii, int column, int line, int shift);
    /// power-on the device
    void poweron(device m);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get info about emulated system
    const char* system_info() const;
    /// called after snapshot restore
    void on_context_switched();
    /// put a key and joystick input (Kempston)
    void put_input(ubyte ascii, ubyte joy0_mask);
    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);
    /// decode the next line into RGBA8Buffer
    void decode_video_line(uint16_t y);
    /// decode audio data
    void decode_audio(float* buffer, int num_samples);

    /// the z80 out callback
    virtual void cpu_out(uword port, ubyte val) override;
    /// the z80 in callback
    virtual ubyte cpu_in(uword port) override;
    /// interrupt request callback
    virtual void irq() override;
    /// clock timer-trigger callback
    virtual void timer(int timer_id) override;

    /// called by timer for each PAL scanline (decodes 1 line of vidmem)
    void scanline();

    static uint32_t palette[8];

    device cur_model = device::zxspectrum48k;
    bool on = false;
    bool memory_paging_disabled = false;
    uint8_t last_fe_out = 0;            // last OUT value to xxFE port
    uint8_t blink_counter = 0;          // increased by one every vblank
    uint16_t scanline_counter = 0;
    uint32_t display_ram_bank = 0;      // which RAM bank to use as display mem
    uint32_t border_color = 0xFF000000;
    uint32_t rgba8_buffer[320*256];
    ubyte joy_mask = 0;                 // joystick mask
    uint64_t next_kbd_mask = 0;
    uint64_t cur_kbd_mask = 0;
    uint64_t key_map[256];              // 8x5 keyboard matrix bits by key code
    sound_beeper beeper;
};

} // namespace YAKC
