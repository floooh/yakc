#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::kc85
    @brief wrapper class for the entire KC85/3 or KC85/4 system
*/
#include "z80.h"
#include "z80pio.h"
#include "roms.h"
#include <stdlib.h>
#include <stdio.h>

namespace yakc {

class kc85 {
public:
    /// ram bank 0 (0x0000..0x3FFF)
    ubyte ram0[0x4000];
    /// ram bank 1 (0x4000..0x7FFF), optional in KC85/3
    ubyte ram1[0x4000];
    /// vidmem bank 0 (0x8000..0xBFFF)
    ubyte irm0[0x4000];

    /// PIO bits
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
        PIO_B_BLINK_ENABLED = (1<<7),
    };

    /// supported KC types
    enum class kc_model {
        kc85_3,
        kc85_4,
        none
    };

    /// the Z80 CPU
    z80 cpu;
    /// the z80 PIO
    z80pio pio;
    /// breakpoint is enabled?
    bool breakpoint_enabled;
    /// breakpoint address
    uword breakpoint_address;
    /// currently paused?
    bool paused;

    /// module infos
    static const int max_num_modules = 256;
    struct {
        ubyte type = 0xFF;
        ubyte control_byte = 0x00;
    } module_info[max_num_modules];

    /// constructor
    kc85();
    /// power-on the device
    void switchon(kc_model m, ubyte* caos_rom, uword caos_rom_size);
    /// power-off the device
    void switchoff();
    /// get pointer to currently mapped rom
    ubyte* caos_rom() const;
    /// get size of currently mapped rom
    uword caos_rom_size() const;
    /// return true if device is switched on
    bool ison() const;
    /// get the KC model
    kc_model model() const;
    /// reset the device
    void reset();
    /// process one frame
    void onframe(int micro_secs);
    /// do one step
    void step();
    /// do a debug-step (executes until PC changes)
    void debug_step();
    /// get current video blink state
    bool blink_state() const;
    /// put a key as ASCII code
    void put_key(ubyte ascii);
    /// handle keyboard input
    void handle_keyboard_input();

private:
    /// the z80 out callback
    static void out_cb(void* userdata, uword port, ubyte val);
    /// the z80 in callback
    static ubyte in_cb(void* userdata, uword port);
    /// fill memory region with noise
    static void fill_noise(void* ptr, int num_bytes);
    /// update module/memory mapping
    void update_bank_switching();

    kc_model cur_model;
    bool on;
    ubyte key_code;
    ubyte* cur_caos_rom;
    uword cur_caos_rom_size;
};

//------------------------------------------------------------------------------
inline kc85::kc85():
breakpoint_enabled(false),
breakpoint_address(0x0000),
paused(false),
cur_model(kc_model::kc85_3),
key_code(0),
cur_caos_rom(rom_caos31),
cur_caos_rom_size(sizeof(rom_caos31)) {
    // empty
}

//------------------------------------------------------------------------------
inline void
kc85::out_cb(void* userdata, uword port, ubyte val) {
    kc85* self = (kc85*)userdata;
    const ubyte module_slot = port>>8;
    switch (port & 0xFF) {
        case 0x80:
            self->module_info[module_slot].control_byte = val;
            self->update_bank_switching();
            break;
        case 0x88:
            self->pio.write(z80pio::A, val);
            self->update_bank_switching();
            break;
        case 0x89:
            self->pio.write(z80pio::B, val);
            break;
        case 0x8A:
            self->pio.control(z80pio::A, val);
            break;
        case 0x8B:
            self->pio.control(z80pio::B, val);
            break;
        case 0x8C:
            // CTC0
            break;
        case 0x8D:
            // CTC1
            break;
        case 0x8E:
            // CTC2
            break;
        case 0x8F:
            // CTC3
        default:
            // unknown
            YAKC_ASSERT(false);
            break;
    }
}

//------------------------------------------------------------------------------
inline ubyte
kc85::in_cb(void* userdata, uword port) {
    kc85* self = (kc85*)userdata;
    const ubyte slot = port >> 8;
    switch (port & 0xFF) {
        case 0x80:
            return self->module_info[slot].type;
        case 0x88:
            return self->pio.read(z80pio::A);
        case 0x89:
            return self->pio.read(z80pio::B);
        default:
            // unknown
            YAKC_ASSERT(false);
            return 0xFF;
    }
}

//------------------------------------------------------------------------------
inline void
kc85::fill_noise(void* ptr, int num_bytes) {
    YAKC_ASSERT((num_bytes & 0x03) == 0);
    unsigned int* uptr = (unsigned int*)ptr;
    int num_uints = num_bytes>>2;
    for (int i = 0; i < num_uints; i++) {
        *uptr++ = rand();
    }
}

//------------------------------------------------------------------------------
inline void
kc85::switchon(kc_model m, ubyte* caos_rom, uword caos_rom_size) {
    YAKC_ASSERT(kc_model::none != m);
    YAKC_ASSERT(!this->on);
    YAKC_ASSERT(0x2000 == caos_rom_size);
    YAKC_ASSERT(0x2000 == sizeof(rom_basic_c0));

    this->cur_model = m;
    this->cur_caos_rom = caos_rom;
    this->cur_caos_rom_size = caos_rom_size;
    this->on = true;
    this->key_code = 0;

    if (kc_model::kc85_3 == m) {
        // fill RAM banks with noise
        fill_noise(this->ram0, sizeof(this->ram0));
        fill_noise(this->irm0, sizeof(this->irm0));

        // initial memory map
        this->cpu.set_inout_handlers(in_cb, out_cb, this);
        this->cpu.out(0x88, 0x9f);
        this->cpu.out(0x89, 0x9f);
        this->update_bank_switching();
    }
    this->pio.init();
    this->cpu.reset();

    // execution on switch-on starts at 0xF000
    this->cpu.state.PC = 0xF000;
}

//------------------------------------------------------------------------------
inline void
kc85::reset() {
    this->pio.reset();
    this->cpu.reset();
    // execution after reset starts at 0xE000
    this->cpu.state.PC = 0xE000;
}

//------------------------------------------------------------------------------
inline void
kc85::switchoff() {
    YAKC_ASSERT(this->on);
    this->cpu.mem.unmap_all();
    this->on = false;
}

//------------------------------------------------------------------------------
inline ubyte*
kc85::caos_rom() const {
    return this->cur_caos_rom;
}

//------------------------------------------------------------------------------
inline uword
kc85::caos_rom_size() const {
    return this->cur_caos_rom_size;
}

//------------------------------------------------------------------------------
inline bool
kc85::ison() const {
    return this->on;
}

//------------------------------------------------------------------------------
inline kc85::kc_model
kc85::model() const {
    return this->cur_model;
}

//------------------------------------------------------------------------------
inline void
kc85::onframe(int micro_secs) {
    // at 1.75 MHz:
    // per micro-second, 1750000 / 1000000 T-states are executed
    // thus: num T-states to execute is (micro_secs * 17500000) / 1000000)
    // or: (micro_secs * 175) / 100
    this->handle_keyboard_input();
    if (!this->paused) {
        const unsigned int num_tstates = (micro_secs * 175) / 100;
        cpu.state.T = 0;
        if (this->breakpoint_enabled) {
            // check if breakpoint has been hit before each cpu-step
            while (cpu.state.T < num_tstates) {
                if (cpu.state.PC == this->breakpoint_address) {
                    this->paused = true;
                    break;
                }
                cpu.step();
            }
        }
        else {
            // fast loop without breakpoint check
            while (cpu.state.T < num_tstates) {
                cpu.step();
            }
        }
    }
}

//------------------------------------------------------------------------------
inline void
kc85::debug_step() {
    uword pc;
    do {
        pc = this->cpu.state.PC;
        cpu.step();
    }
    while ((pc == cpu.state.PC) && !cpu.state.INV);
}

//------------------------------------------------------------------------------
inline void
kc85::step() {
    this->cpu.step();
}

//------------------------------------------------------------------------------
inline bool
kc85::blink_state() const {
    // FIXME
    return true;
}

//------------------------------------------------------------------------------
inline void
kc85::put_key(ubyte ascii) {
    this->key_code = ascii;
}

//------------------------------------------------------------------------------
inline void
kc85::handle_keyboard_input() {
    // NOTE: this is a shortcut and doesn't emulate the hardware's
    // tricky serial keyboard input. Instead we're directly poking
    // the ASCII value into the right memory location

    // don't do anything if interrupts disabled, IX might point
    // to the wrong base address!
    if (!this->cpu.state.IFF1) {
        return;
    }

    // don't do anything if previous key hasn't been read yet
    // NOTE: IX is usually set to 0x01F0
    const uword ix = this->cpu.state.IX;
    if (0 != (this->cpu.mem.r8(ix+0x08) & 1)) {
        return;
    }

    // write key to special OS locations
    if (0 == this->key_code) {
        // clear ascii code location
        this->cpu.mem.w8(ix+0x0D, 0);
        // clear repeat count
        this->cpu.mem.w8(ix+0x0A, 0);
    }
    else {
        bool overwrite = true;
        // FIXME: handle repeat
        if (this->cpu.mem.r8(ix+0x0D) == this->key_code) {
            // FIXME!
        }
        if (overwrite) {
            this->cpu.mem.w8(ix+0x0D, this->key_code);
            this->cpu.mem.w8(ix+0x08, this->cpu.mem.r8(ix+0x08)|1);
        }
        this->key_code = 0;
    }
}

//------------------------------------------------------------------------------
inline void
kc85::update_bank_switching() {
    // this is called from the out-handler to update modules and memory banks
    const ubyte pio_a = this->pio.read(z80pio::A);

    // 0x0000
    if (pio_a & PIO_A_RAM) {
        // built-in RAM switched on        
        this->cpu.mem.map(0x0000, 0x4000, this->ram0, true);
    }
    // FIXME: handle extension modules!
    else {
        // nothing mapped to 0x0000
        this->cpu.mem.unmap(0x0000, 0x4000);
    }
    // 0x4000
    // FIXME!

    // 0x8000 (IRM)
    if (pio_a & PIO_A_IRM) {
        // built-in IRM (video-mem) switched on
        this->cpu.mem.map(0x8000, 0x4000, this->irm0, true);
    }
    // FIXME: handle extension modules!
    else {
        // nothing mapped to 0x8000
        this->cpu.mem.unmap(0x8000, 0x4000);
    }

    // 0xC000 (BASIC ROM)
    if (pio_a & PIO_A_BASIC_ROM) {
        // BASIC is switched on
        this->cpu.mem.map(0xC000, 0x2000, rom_basic_c0, false);
    }
    // FIXME: handle extension modules!
    else {
        // nothing mapped to 0xC000 address
        this->cpu.mem.unmap(0xC000, 0x2000);
    }

    // 0xF000 (CAOS ROM)
    if (pio_a & PIO_A_CAOS_ROM) {
        // CAOS ROM switched on
        this->cpu.mem.map(0xE000, 0x2000, this->caos_rom(), false);
    }
    // FIXME: handle extension modules!
    else {
        // nothing mapped to 0xE000 address
        this->cpu.mem.unmap(0xE000, 0x2000);
    }
}

} // namespace yakc
