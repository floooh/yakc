#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::breadboard
    @brief houses all the chips required by emulated systems
*/
#include "yakc/common.h"
#include "yakc/clock.h"
#include "yakc/z80.h"
#include "yakc/z80dbg.h"
#include "yakc/z80pio.h"
#include "yakc/z80ctc.h"

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
