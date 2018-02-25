#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::c64
    @brief Commodore C64 emulation
*/
#include "yakc/util/breadboard.h"
#include "yakc/util/rom_images.h"

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
    /// callback for m6510 port input
    static uint8_t cpu_port_in();
    /// callback for M6510 port output
    static void cpu_port_out(uint8_t data);
    /// VIC-II memory fetch callback
    static uint64_t vic_fetch(uint64_t pins);

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

    bool on = false;
    system model = system::c64_pal;
    uint8_t cpu_port = 0;
    bool io_mapped = false;
};
extern c64_t c64;

} // namespace YAKC


