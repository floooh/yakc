#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::kc85
    @brief wrapper class for the KC85/2, /3, /4
*/
#include "yakc/breadboard.h"
#include "yakc/roms.h"
#include "yakc/kc85_video.h"
#include "yakc/kc85_audio.h"
#include "yakc/kc85_exp.h"
#include "yakc/kc85_roms.h"

namespace YAKC {

class kc85 {
public:
    /// ram banks
    ubyte ram[4][0x4000];

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
    kc85_video video;
    kc85_audio audio;
    kc85_exp exp;
    kc85_roms roms;
    ubyte io84 = 0;         // special KC85/4 io register
    ubyte io86 = 0;         // special KC85/4 io register

    /// one-time init
    void init(breadboard* board);

    /// power-on the device
    void poweron(device m, os_rom os);
    /// power-off the device
    void poweroff();
    /// reset the device
    void reset();
    /// get the KC model
    device model() const;
    /// get the CAOS version
    os_rom caos() const;

    /// process one frame, up to absolute number of cycles
    void onframe(int speed_multiplier, int micro_secs, uint64_t min_cycle_count, uint64_t max_cycle_count);
    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// handle keyboard input
    void handle_keyboard_input();

    /// the z80 out callback
    static void out_cb(void* userdata, uword port, ubyte val);
    /// the z80 in callback
    static ubyte in_cb(void* userdata, uword port);
    /// update module/memory mapping
    void update_bank_switching();
    /// update the rom pointers
    void update_rom_pointers();

    device cur_model = device::kc85_3;
    os_rom cur_caos = os_rom::caos_3_1;
    bool on = false;
    bool cpu_ahead = false;                 // cpu would have been ahead of max_cycle_count
    bool cpu_behind = false;                // cpu would have been behind of min_cycle_count
    uint64_t abs_cycle_count = 0;           // total CPU cycle count
    uint32_t overflow_cycles = 0;           // cycles that have overflowed from last frame
    ubyte key_code = 0;
    const ubyte* caos_c_ptr = nullptr;
    int caos_c_size = 0;
    const ubyte* caos_e_ptr = nullptr;
    int caos_e_size = 0;
};

} // namespace YAKC

