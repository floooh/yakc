#pragma once
//------------------------------------------------------------------------------
/** 
    @class YAKC::atom
    @brief Acorn Atom emulation
    
    https://fjkraan.home.xs4all.nl/comp/atom/index.html
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

    /// check if required roms are loaded
    static bool check_roms(const rom_images& roms, system model, os_rom os);

    /// one-time setup
    void init(breadboard* board, rom_images* roms);
    /// initialize the keyboard matrix mapping table
    void init_keymap();
    /// initialize a single entry in the key-map table
    void init_key_mask(uint8_t ascii, int column, int line, bool shift=false, bool ctrl=false);

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
    /// put a key and joystick input
    void put_input(uint8_t ascii);

    /// memory-mapped-io callback
    static uint8_t memio(bool write, uint16_t addr, uint8_t inval);
    /// vidmem reader function (called from mc6847)
    static uint8_t read_vidmem(uint16_t addr);
    /// called by cycle-stepped CPU per 'subcycle'
    virtual void cpu_tick() override;
    /// PIO output callback
    virtual void pio_out(int pio_id, int port_id, uint8_t val) override;
    /// PIO input callback
    virtual uint8_t pio_in(int pio_id, int port_id) override;

    static atom* self;
    bool on = false;
    const uint8_t* vidmem_base = nullptr;
    mc6847* vdg = nullptr;

    // keyboard matrix has 10 columns @ 8 rows,
    // complete row 6 is ctrl
    // complete row 7 is shift
    struct key_mask {
        static const int num_cols = 10;
        uint8_t col[num_cols];
        void combine(const key_mask& m) {
            for (int i = 0; i < num_cols; i++) {
                this->col[i] |= m.col[i];
            }
        }
        void clear(const key_mask& m) {
            for (int i = 0; i < num_cols; i++) {
                this->col[i] &= ~m.col[i];
            }
        }
    };
    uint8_t scan_kbd_col = 0;   // next keyboard column to be scanned (0..10)
    key_mask next_key_mask;
    key_mask cur_key_mask;
    key_mask key_map[256];
};

} // namespace YAKC
