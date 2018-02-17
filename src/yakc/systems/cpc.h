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
#include "yakc/systems/cpc_video.h"

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

    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);
    /// perform a single debug-step
    uint32_t step_debug();
    /// the Z80 CPU tick callback
    static uint64_t cpu_tick(int num_ticks, uint64_t pins);
    /// i8255 output callback
    static uint64_t ppi_out(int port_id, uint64_t pins, uint8_t data);
    /// i8255 input callback
    static uint8_t ppi_in(int port_id);
    /// tick the gate array
    uint64_t ga_tick(uint64_t pins);
    /// perform a CPU OUT
    void cpu_out(uint64_t pins);
    /// perform a CPU IN
    uint8_t cpu_in(uint64_t pins);

    /// update bank switching
    void update_memory_mapping();
    /// initialize the keycode translation map
    void init_keymap();

    /// put a key and joystick input
    void put_input(uint8_t ascii, uint8_t joy0_mask);
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
    cpc_video video;
    uint32_t tick_count = 0;
    uint16_t casread_trap = 0x0000;
    uint16_t casread_ret = 0x0000;

    uint8_t ga_config = 0x00;     // out to port 0x7Fxx func 0x80
    uint8_t ram_config = 0x00;    // out to port 0x7Fxx func 0xC0
};
extern cpc_t cpc;

} // namespace YAKC
