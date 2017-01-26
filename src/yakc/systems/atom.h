#pragma once
//------------------------------------------------------------------------------
/** 
    @class YAKC::atom
    @brief Acorn Atom emulation
*/
#include "yakc/core/system_bus.h"
#include "yakc/systems/breadboard.h"
#include "yakc/systems/rom_images.h"

namespace YAKC {

class atom : public system_bus {
public:
    /// the main board
    breadboard* board = nullptr;
    /// rom image storage
    rom_images* roms = nullptr;

    /// one-time setup
    void init(breadboard* board, rom_images* roms);
    /// check if required roms are loaded
    static bool check_roms(const rom_images& roms, device model, os_rom os);    
    /// power-on the device
    void poweron();
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);

    /// get framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// get info about emulated system
    const char* system_info() const;
    /// called after snapshot restore
    void on_context_switched();    
    /// put a key and joystick input (Kempston)
    void put_input(uint8_t ascii, uint8_t joy0_mask);

    /// memory-mapped-io callback
    static uint8_t memio(bool write, uint16_t addr, uint8_t inval);
    /// called by cycle-stepped CPU per 'subcycle'
    virtual void cpu_tick() override;
    /// PIO output callback
    virtual void pio_out(int pio_id, int port_id, uint8_t val) override;
    /// PIO input callback
    virtual uint8_t pio_in(int pio_id, int port_id) override;

    static atom* self;
    bool on = false;
};

} // namespace YAKC
