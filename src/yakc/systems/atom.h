#pragma once
//------------------------------------------------------------------------------
/** 
    @class YAKC::atom
    @brief Acorn Atom emulation
    
    https://fjkraan.home.xs4all.nl/comp/atom/index.html
*/
#include "yakc/systems/breadboard.h"
#include "yakc/core/filesystem.h"
#include "yakc/systems/rom_images.h"
#include "yakc/peripherals/tapedeck.h"

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

    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);
    /// perform a single debug-step
    uint32_t step_debug();
    /// the m6502 tick callback
    static uint64_t cpu_tick(uint64_t pins);
    /// the MC6847 video memory fetch callback
    static uint64_t vdg_fetch(uint64_t pins);
    /// i8255 input callback
    static uint8_t ppi_in(int port_id);
    /// i8255 output callback
    static uint64_t ppi_out(int port_id, uint64_t pins, uint8_t data);
    /// m6522 input callback
    static uint8_t via_in(int port_id);
    /// m6522 output callback
    static void via_out(int port_id, uint8_t data);

    /// get framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// decode audio data
    void decode_audio(float* buffer, int num_samples);    
    /// put a key and joystick input
    void put_input(uint8_t ascii, uint8_t joy0mask);
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
