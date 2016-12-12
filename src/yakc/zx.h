#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::zx
    @brief wrapper class for Sinclair ZX emulation
*/
#include "yakc/breadboard.h"
#include "roms/roms.h"
#include "yakc/z80bus.h"

namespace YAKC {

class zx : public z80bus {
public:
    /// ram banks
    ubyte ram[8][0x4000];

    /// the main board
    breadboard* board = nullptr;

    /// one-time setup
    void init(breadboard* board);
    /// initialize the memory map
    void init_memory_mapping();
    /// power-on the device
    void poweron(device m);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get the Z1013 model
    device model() const;
    /// get info about emulated system
    const char* system_info() const;
    /// called after snapshot restore
    void on_context_switched();
    /// get current border color
    void border_color(float& out_red, float& out_green, float& out_blue);

    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);

    /// handle (hacked) keyboard input
    void handle_keyboard_input();
    /// decode an entire frame into RGBA8Buffer
    void decode_video();

    /// the z80 out callback
    virtual void cpu_out(uword port, ubyte val) override;
    /// the z80 in callback
    virtual ubyte cpu_in(uword port) override;
    /// interrupt request callback
    virtual void irq() override;

    device cur_model = device::zxspectrum48k;
    os_rom cur_os = os_rom::amstrad_zx48k;
    bool on = false;
    uint8_t key_code = 0;
    uint32_t display_ram_bank = 5;      // which RAM bank to use as display mem
    uint32_t brder_color = 0;
    uint32_t pal[8];
    uint32_t rgba8_buffer[256*192];
};

} // namespace YAKC
