#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::kc85
    @brief wrapper class for the KC85/2, /3, /4
*/
#include "yakc/breadboard.h"
#include "yakc/rom_images.h"
#include "yakc/system_bus.h"
#include "yakc/kc85_video.h"
#include "yakc/kc85_audio.h"
#include "yakc/kc85_exp.h"

namespace YAKC {

class kc85 : public system_bus {
public:
    /// IO bits
    enum {
        PIO_A_CAOS_ROM    = (1<<0),
        PIO_A_RAM         = (1<<1),
        PIO_A_IRM         = (1<<2),
        PIO_A_RAM_RO      = (1<<3),
        PIO_A_UNUSED      = (1<<4),
        PIO_A_TAPE_LED    = (1<<5),
        PIO_A_TAPE_MOTOR  = (1<<6),
        PIO_A_BASIC_ROM   = (1<<7),

        PIO_B_VOLUME_MASK = (1<<5)-1,
        PIO_B_RAM8 = (1<<5),            // KC85/4 only
        PIO_B_RAM8_RO = (1<<6),         // KC85/4 only
        PIO_B_BLINK_ENABLED = (1<<7),

        // KC85/4 only
        IO84_SEL_VIEW_IMG   = (1<<0),  // 0: display img0, 1: display img1
        IO84_SEL_CPU_COLOR  = (1<<1),  // 0: access pixels, 1: access colors
        IO84_SEL_CPU_IMG    = (1<<2),  // 0: access img0, 1: access img1
        IO84_HICOLOR        = (1<<3),  // 0: hicolor mode off, 1: hicolor mode on
        IO84_SEL_RAM8       = (1<<4),  // select RAM8 block 0 or 1
        IO84_BLOCKSEL_RAM8  = (1<<5),  // no idea what that does...?

        IO86_RAM4   = (1<<0),
        IO86_RAM4_RO = (1<<1),
        IO86_CAOS_ROM_C = (1<<7)
    };

    /// hardware components
    breadboard* board = nullptr;
    rom_images* roms = nullptr;
    kc85_video video;
    kc85_audio audio;
    kc85_exp exp;
    ubyte pio_a = 0;        // backing for PIO-A data
    ubyte pio_b = 0;        // backing for PIO-B data
    ubyte io84 = 0;         // special KC85/4 io register
    ubyte io86 = 0;         // special KC85/4 io register

    /// one-time init
    void init(breadboard* board, rom_images* roms);
    /// check if required roms are loaded
    static bool check_roms(const rom_images& roms, device model, os_rom os);

    /// power-on the device
    void poweron(device m, os_rom os);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get info about emulated system
    const char* system_info() const;
    /// called after snapshot restore
    void on_context_switched();
    /// decode audio data
    void decode_audio(float* buffer, int num_samples);
    /// get pointer to framebuffer, width and height
    const void* framebuffer(int& out_width, int& out_height);

    /// process a number of cycles, return final processed tick
    uint64_t step(uint64_t start_tick, uint64_t end_tick);
    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// handle keyboard input
    void handle_keyboard_input();

    /// the z80 out callback
    virtual void cpu_out(uword port, ubyte val) override;
    /// the z80 in callback
    virtual ubyte cpu_in(uword port) override;
    /// CTC write callback
    virtual void ctc_write(int ctc_id, int chn_id) override;
    /// CTC zcto callback
    virtual void ctc_zcto(int ctc_id, int chn_id) override;
    /// Z80 PIO input callback
    virtual ubyte pio_in(int pio_id, int port_id) override;
    /// Z80 PIO output callback
    virtual void pio_out(int pio_id, int port_id, ubyte val) override;
    /// interrupt request callback
    virtual void irq() override;
    /// clock timer-trigger callback
    virtual void timer(int timer_id) override;

    /// update module/memory mapping
    void update_bank_switching();
    /// update the rom pointers
    void update_rom_pointers();

    device cur_model = device::kc85_3;
    os_rom cur_caos = os_rom::caos_3_1;
    bool on = false;
    ubyte key_code = 0;
    ubyte* caos_c_ptr = nullptr;
    int caos_c_size = 0;
    ubyte* caos_e_ptr = nullptr;
    int caos_e_size = 0;
    ubyte* basic_ptr = nullptr;
    int basic_size = 0;
};

} // namespace YAKC

