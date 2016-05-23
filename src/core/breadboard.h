#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::breadboard
    @brief houses all the chips required by emulated systems
*/
#include "core/common.h"
#include "core/clock.h"
#include "core/z80.h"
#include "core/z80dbg.h"
#include "core/z80pio.h"
#include "core/z80ctc.h"

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
