#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::breadboard
    @brief houses all the chips required by emulated systems
*/
#include "core/common.h"
#include "core/clock.h"
#include "z80/z80.h"
#include "z80/z80dbg.h"
#include "z80/z80pio.h"
#include "z80/z80ctc.h"

namespace yakc {

class breadboard {
public:
    clock clck;
    z80 cpu;
    z80pio pio;
    z80ctc ctc;
    z80dbg dbg;
};

} // namespace yakc
