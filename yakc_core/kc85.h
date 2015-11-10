#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::kc85
    @brief wrapper class for the entire KC85/3 or KC85/4 system
*/
#include "z80.h"
#include "roms.h"
#include <stdlib.h>
#include <stdio.h>

namespace yakc {

class kc85 {
public:
    /// ram bank 0 (0x0000..0x3FFF)
    ubyte ram0[memory::bank::size];
    /// ram bank 1 (0x4000..0x7FFF), optional in KC85/3
    ubyte ram1[memory::bank::size];
    /// vidmem bank 0 (0x8000..0xBFFF)
    ubyte irm0[memory::bank::size];
    /// vidmem bank 1 (0x8000..0xBFFF), only in KC85/4
    ubyte irm1[memory::bank::size];
    /// rom bank 1 (0xC000..0xFFFF)
    ubyte rom0[memory::bank::size];

    /// the Z80 CPU
    z80 cpu;
    /// breakpoint is enabled?
    bool breakpoint_enabled;
    /// breakpoint address
    uword breakpoint_address;
    /// currently paused?
    bool paused;

    /// supported KC types
    enum class kc_model {
        kc85_3,
        kc85_4,
        none
    };

    /// constructor
    kc85();
    /// the z80 out callback
    static void out_cb(void* userdata, uword port, ubyte val);
    /// the z80 in callback
    static ubyte in_cb(void* userdata, uword port);
    /// fill memory region with noise
    static void fill_noise(void* ptr, int num_bytes);
    /// power-on the device
    void switchon(kc_model m);
    /// power-off the device
    void switchoff();
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
    kc_model cur_model;
    bool on;
    ubyte key_code;
};

//------------------------------------------------------------------------------
inline kc85::kc85():
breakpoint_enabled(false),
breakpoint_address(0x0000),
paused(false),
cur_model(kc_model::none),
key_code(0) {
    // empty
}

//------------------------------------------------------------------------------
inline void
kc85::out_cb(void* userdata, uword port, ubyte val) {
    // FIXME
}

//------------------------------------------------------------------------------
inline ubyte
kc85::in_cb(void* userdata, uword port) {
    // FIXME
    return 0;
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
kc85::switchon(kc_model m) {
    YAKC_ASSERT(kc_model::none != m);
    YAKC_ASSERT(!this->on);
    this->cur_model = m;
    this->on = true;
    this->key_code = 0;

    if (kc_model::kc85_3 == m) {
        // copy ROM content
        YAKC_ASSERT(8192 == sizeof(rom_basic_c0));
        YAKC_ASSERT(8192 == sizeof(rom_caos31));

        // copy the ROM content
        memcpy(this->rom0, rom_basic_c0, sizeof(rom_basic_c0));
        memcpy(this->rom0 + sizeof(rom_basic_c0), rom_caos31, sizeof(rom_caos31));

        // fill RAM banks with noise
        fill_noise(this->ram0, sizeof(this->ram0));
        fill_noise(this->irm0, sizeof(this->irm0));

        this->cpu.mem.map(0, this->ram0, sizeof(this->ram0), memory::type::ram);
        this->cpu.mem.map(2, this->irm0, sizeof(this->irm0), memory::type::ram);
        this->cpu.mem.map(3, this->rom0, sizeof(this->rom0), memory::type::rom);
        this->cpu.set_inout_handlers(in_cb, out_cb, this);
    }
    this->cpu.reset();

    // execution on switch-on starts at 0xF000
    this->cpu.state.PC = 0xF000;
}

//------------------------------------------------------------------------------
inline void
kc85::reset() {
    this->cpu.reset();
    // execution after reset starts at 0xE000
    this->cpu.state.PC = 0xE000;
}

//------------------------------------------------------------------------------
inline void
kc85::switchoff() {
    YAKC_ASSERT(this->on);
    for (int i = 0; i < memory::num_banks; i++) {
        this->cpu.mem.unmap(i);
    }
    this->on = false;
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

} // namespace yakc
