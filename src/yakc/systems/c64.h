#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::c64
    @brief Commodore C64 emulation
*/
#include "yakc/util/breadboard.h"
#include "yakc/util/rom_images.h"
#include "yakc/util/filetypes.h"

namespace YAKC {

class c64_t {
public:
    /// check if required roms are loaded
    static bool check_roms(system model, os_rom os);

    /// power-on the device
    void poweron(system model);
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

    /// the m6502 tick callback
    static uint64_t cpu_tick(uint64_t pins);
    /// CPU port input callback
    static uint8_t cpu_port_in();
    /// CPU port output callback
    static void cpu_port_out(uint8_t data);
    /// CIA-1 port output callback
    static void cia1_out(int port_id, uint8_t data);
    /// CIA-1 port input callback
    static uint8_t cia1_in(int port_id);
    /// CIA-2 port output callback
    static void cia2_out(int port_id, uint8_t data);
    /// CIA-2 port input callback
    static uint8_t cia2_in(int port_id);

    /// VIC-II memory fetch callback (returns a 4+8 bits value)
    static uint16_t vic_fetch(uint16_t addr);

    /// setup the keyboard matrix
    void init_keymap();
    /// called when alpha-numeric key has been pressed
    void on_ascii(uint8_t ascii);
    /// called when non-alnum key has been pressed down
    void on_key_down(uint8_t key);
    /// called when non-alnum key has been released
    void on_key_up(uint8_t key);
    /// called for joystick input
    void on_joystick(uint8_t mask);
    /// setup the initial memory configuration
    void init_memory_map();
    /// update the current memory configuration
    void update_memory_map();
    /// get framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// decode audio data
    void decode_audio(float* buffer, int num_samples);

    /// called when a tape has been inserted into the tape deck
    void on_tape_inserted();
    /// tick the datasette, return true if CIA-1 FLAG must be triggered
    bool tape_tick();

    bool on = false;
    system model = system::c64_pal;
    uint8_t cpu_port = 0;
    uint8_t cia1_joy_bits = 0;
    uint16_t vic_bank_select = 0;   // upper 4 address bits from CIA-2 port A
    bool io_mapped = false;
    uint8_t color_ram[1024];    // special static color ram

    // tape emulation
    c64tap_header tap_header;
    bool tape_valid = false;
    uint32_t tape_tick_count = 0;
    int32_t tape_byte_count = 0;
};
extern c64_t c64;

} // namespace YAKC


