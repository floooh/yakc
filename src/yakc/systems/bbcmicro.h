#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::bbcmicro
    @brief BBC Micro emulator wrapper
*/
#include "yakc/core/system_bus.h"
#include "yakc/systems/breadboard.h"
#include "yakc/systems/rom_images.h"
#include "yakc/systems/bbcmicro_video.h"

namespace YAKC {

class bbcmicro : public system_bus {
public:
    /// the main board
    breadboard* board = nullptr;
    /// rom image store
    rom_images* roms = nullptr;

    /// one-time setup
    void init(breadboard* board, rom_images* roms);
    /// check if required roms are loaded
    static bool check_roms(const rom_images& roms, system model, os_rom os);
    /// initialize memory mapping (called from poweron or snapshot restore)
    void init_memory_map();
    
    /// power-on the device
    void poweron(system m);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get info about emulated system
    const char* system_info() const;
    /// called after snapshot restore
    void on_context_switched();
    /// get framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);

    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);
    /// perform a single debug-step
    uint32_t step_debug();

    /// memory-mapped-io callback
    static uint8_t memio(bool write, uint16_t addr, uint8_t inval);
    /// called by cycle-stepped CPU per 'subcycle'
    virtual void cpu_tick();

    static bbcmicro* self;
    system cur_model = system::bbcmicro_b;
    bool on = false;
    bbcmicro_video video;
};

} // namespace YAKC

