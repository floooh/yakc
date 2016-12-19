#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::breadboard
    @brief houses all the common chips required by emulated systems
*/
#include "yakc/core.h"
#include "yakc/clock.h"
#include "yakc/z80.h"
#include "yakc/z80dbg.h"
#include "yakc/z80pio.h"
#include "yakc/z80ctc.h"

namespace YAKC {

class breadboard {
public:
    clock clck;
    z80 cpu;
    z80pio pio;
    z80pio pio2;
    z80ctc ctc;
    z80dbg dbg;
    ubyte ram[8][0x4000];
};

} // namespace YAKC
