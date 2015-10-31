#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::kc85
    @brief wrapper class for the entire KC85/3 or KC85/4 system
*/
#include "z80.h"

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
    kc85();

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

    /// pause/continue execution
    void pause(bool paused);
    /// return true if currently paused
    bool paused() const;
    /// execute next instruction
    void step();

    /// get current video blink state
    bool blink_state() const;

private:
    kc_model cur_model;
    bool on;
    bool is_paused;
};

} // namespace yakc
