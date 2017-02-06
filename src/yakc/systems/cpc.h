#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::cpc
    @brief Amstrad CPC 464/6128 and KC Compact emulation
*/
#include "yakc/core/system_bus.h"
#include "yakc/systems/breadboard.h"
#include "yakc/systems/rom_images.h"
#include "yakc/core/filesystem.h"
#include "yakc/systems/cpc_video.h"

namespace YAKC {

class cpc : public system_bus {
public:
    /// the main board
    breadboard* board = nullptr;
    /// rom image storage
    rom_images* roms = nullptr;

    /// one-time setup
    void init(breadboard* board, rom_images* roms);
    /// check if required roms are loaded
    static bool check_roms(const rom_images& roms, system model, os_rom os);
    /// initialize the memory map
    void init_memory_map();
    /// initialize the keycode translation map
    void init_keymap();
    /// initialize a single entry in the key-map table
    void init_key_mask(uint8_t ascii, int column, int line, int shift);
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
    /// put a key and joystick input
    void put_input(uint8_t ascii, uint8_t joy0_mask);

    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);
    /// perform a single debug-step
    uint32_t step_debug();

    /// update bank switching
    void update_memory_mapping();
    /// decode next audio buffer
    void decode_audio(float* buffer, int num_samples);
    /// get pointer to framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);
    /// file quickloading
    bool quickload(filesystem* fs, const char* name, filetype type, bool start);
    /// load an SNA snapshot file
    bool load_sna(filesystem* fs, const char* name, filetype type, bool start);
    /// start loading a TAP file
    bool load_tap(filesystem* fs, const char* name, filetype type, bool start);
    /// the trapped casread function for TAP files
    void casread();

    /// the z80 out callback
    virtual void cpu_out(uint16_t port, uint8_t val) override;
    /// the z80 in callback
    virtual uint8_t cpu_in(uint16_t port) override;
    /// PIO output callback
    virtual void pio_out(int pio_id, int port_id, uint8_t val) override;
    /// PIO input callback
    virtual uint8_t pio_in(int pio_id, int port_id) override;
    /// interrupt request callback
    virtual void irq(bool b) override;
    /// interrupt acknowledge callback
    virtual void iack() override;
    /// vblank callback
    virtual void vblank() override;

    system cur_model = system::cpc464;
    bool on = false;
    uint16_t casread_trap = 0x0000;
    uint16_t casread_ret = 0x0000;
    filesystem::file tap_fp = filesystem::invalid_file;
    filesystem* tap_fs = nullptr;

    cpc_video video;

    uint8_t psg_selected = 0;     // selected AY8910 register
    uint8_t ga_config = 0x00;     // out to port 0x7Fxx func 0x80
    uint8_t ram_config = 0x00;    // out to port 0x7Fxx func 0xC0
    struct key_mask {
        static const int num_lines = 10;
        uint8_t col[num_lines] = { };
        void combine(const key_mask& m) {
            for (int i = 0; i < num_lines; i++) {
                this->col[i] |= m.col[i];
            }
        }
        void clear(const key_mask& m) {
            for (int i = 0; i < num_lines; i++) {
                this->col[i] &= ~m.col[i];
            }
        }
    };
    uint8_t scan_kbd_line = 0;    // next keyboard line to be scanned
    key_mask next_key_mask;
    key_mask next_joy_mask;
    key_mask cur_key_mask;
    key_mask key_map[256];
};

} // namespace YAKC
