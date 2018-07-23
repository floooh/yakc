#pragma once
//------------------------------------------------------------------------------
/** 
    @class YAKC::atom
    @brief Acorn Atom emulation
    
    https://fjkraan.home.xs4all.nl/comp/atom/index.html
*/
#include "yakc/util/breadboard.h"
#include "yakc/util/rom_images.h"
#include "yakc/util/filesystem.h"
#include "yakc/util/tapedeck.h"

namespace YAKC {

class atom_t {
public:
    /// check if required roms are loaded
    static bool check_roms(system model, os_rom os);

    /// power-on the device
    void poweron();
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
    static uint64_t cpu_tick(uint64_t pins, void* user_data);
    /// the MC6847 video memory fetch callback
    static uint64_t vdg_fetch(uint64_t pins, void* user_data);
    /// i8255 input callback
    static uint8_t ppi_in(int port_id, void* user_data);
    /// i8255 output callback
    static uint64_t ppi_out(int port_id, uint64_t pins, uint8_t data, void* user_data);
    /// m6522 input callback
    static uint8_t via_in(int port_id, void* user_data);
    /// m6522 output callback
    static void via_out(int port_id, uint8_t data, void* user_data);

    /// called when alpha-numeric key has been pressed
    void on_ascii(uint8_t ascii);
    /// called when non-alnum key has been pressed down
    void on_key_down(uint8_t key);
    /// called when non-alnum key has been released
    void on_key_up(uint8_t key);
    /// called for joystick input
    void on_joystick(uint8_t mask);
    /// get framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// decode audio data
    void decode_audio(float* buffer, int num_samples);    
    /// file quickloading
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);    
    /// the trapped osload() function for TAP files
    void osload();
    /// initialize the keyboard matrix mapping table
    void init_keymap();
    /// initialize a single entry in the key-map table
    void init_key_mask(uint8_t ascii, int column, int line, bool shift=false, bool ctrl=false);

    bool on = false;
    const uint8_t* vidmem_base = nullptr;
    int count_2_4khz;
    int period_2_4khz;
    bool state_2_4khz = false;
    bool out_cass0 = false;
    bool out_cass1 = false;

    // FIXME: implement a proper AtomMMC emulation
    uint8_t mmc_joymask = 0;
    uint8_t mmc_cmd = 0;
    uint8_t mmc_latch = 0;
};
extern atom_t atom;

} // namespace YAKC
