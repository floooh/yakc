#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::kc85
    @brief wrapper class for the entire KC85/3 or KC85/4 system
*/
#include "z80.h"
#include "roms.h"
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

    /// supported KC types
    enum class kc_model {
        kc85_3,
        kc85_4,
        none
    };

    /// constructor
    kc85():
        cur_model(kc_model::none),
        is_paused(false) {
        memset(this->ram0, 0, sizeof(this->ram0));
        memset(this->ram1, 0, sizeof(this->ram1));
        memset(this->irm0, 0, sizeof(this->irm0));
        memset(this->irm1, 0, sizeof(this->irm1));
        memset(this->rom0, 0, sizeof(this->rom0));
    }
    /// the z80 out callback
    static void out_cb(void* userdata, uword port, ubyte val) {
        printf("out 0x%04X: 0x%02X\n", port, val);
    }
    /// the z80 in callback
    static ubyte in_cb(void* userdata, uword port) {
        printf("in 0x%04X\n", port);
        return 0;
    }
    /// power-on the device
    void switchon(kc_model m) {
        YAKC_ASSERT(kc_model::none != m);
        YAKC_ASSERT(!this->on);
        this->cur_model = m;
        this->on = true;

        if (kc_model::kc85_3 == m) {
            // copy ROM content
            YAKC_ASSERT(8192 == sizeof(rom_basic_c0));
            YAKC_ASSERT(8192 == sizeof(rom_caos31));
            memcpy(this->rom0, rom_basic_c0, sizeof(rom_basic_c0));
            memcpy(this->rom0 + sizeof(rom_basic_c0), rom_caos31, sizeof(rom_caos31));

            this->cpu.mem.map(0, this->ram0, sizeof(this->ram0), memory::type::ram);
            this->cpu.mem.map(2, this->irm0, sizeof(this->irm0), memory::type::ram);
            this->cpu.mem.map(3, this->rom0, sizeof(this->rom0), memory::type::rom);
            this->cpu.set_inout_handlers(in_cb, out_cb, this);
        }
        this->cpu.reset();

        // execution on switch-on starts at 0xF000
        this->cpu.state.PC = 0xF000;
    }
    /// power-off the device
    void switchoff() {
        YAKC_ASSERT(this->on);
        this->on = false;
    }
    /// return true if device is switched on
    bool ison() const {
        return this->on;
    }
    /// get the KC model
    kc_model model() const {
        return this->cur_model;
    }
    /// reset the device
    void reset() {
        this->cpu.reset();
    }
    /// process one frame
    void onframe(int micro_secs) {
        // at 1.75 MHz:
        // per micro-second, 1750000 / 1000000 T-states are executed
        // thus: num T-states to execute is (micro_secs * 17500000) / 1000000)
        // or: (micro_secs * 175) / 100
        if (!this->is_paused) {
            unsigned int num_tstates = (micro_secs * 175) / 100;
            this->cpu.run(num_tstates);
        }
    }
    /// pause/continue execution
    void pause(bool paused) {
        this->is_paused = paused;
    }
    /// return true if currently paused
    bool paused() const {
        return this->is_paused;
    }
    /// execute next instruction
    void step() {
        this->cpu.step();
    }
    /// get current video blink state
    bool blink_state() const {
        // FIXME
        return true;
    }

private:
    kc_model cur_model;
    bool on;
    bool is_paused;
};

} // namespace yakc
