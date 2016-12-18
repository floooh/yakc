#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::cpc
    @brief Amstrad CPC 464/6128 and KC Compact emulation
*/
#include "yakc/breadboard.h"
#include "yakc/rom_images.h"
#include "yakc/z80bus.h"
#include "yakc/cpc_video.h"

namespace YAKC {

class cpc : public z80bus {
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
    /// initialize the keycode translation map
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
    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);

    /// the z80 out callback
    virtual void cpu_out(uword port, ubyte val) override;
    /// the z80 in callback
    virtual ubyte cpu_in(uword port) override;
    /// interrupt request callback
    virtual void irq() override;
    /// vblank callback
    virtual void vblank() override;

    device cur_model = device::cpc464;
    bool on = false;
    cpc_video video;
    struct key_mask {
        ubyte col[10] = { };
    };
    key_mask next_key_mask;
    key_mask cur_key_mask;
    key_mask key_map[256];
};

} // namespace YAKC
