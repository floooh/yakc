#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::breadboard
    @brief houses all the common chips required by emulated systems
*/
#include "yakc/core/core.h"
#include "yakc/core/clock.h"
#include "yakc/devices/z80.h"
#include "yakc/devices/mos6502.h"
#include "yakc/devices/cpudbg.h"
#include "yakc/devices/z80pio.h"
#include "yakc/devices/z80ctc.h"
#include "yakc/devices/i8255.h"
#include "yakc/devices/mc6845.h"
#include "yakc/devices/mc6847.h"
#include "yakc/devices/beeper.h"
#include "yakc/devices/speaker.h"
#include "yakc/devices/ay8910.h"
#include "yakc/devices/crt.h"

namespace YAKC {

class breadboard {
public:
    clock clck;
    class z80 z80;
    class mos6502 mos6502;
    class z80pio z80pio;
    class z80pio z80pio2;
    class z80ctc z80ctc;
    class i8255 i8255;
    class mc6845 mc6845;
    class mc6847 mc6847;
    class beeper beeper;
    class speaker speaker;
    class ay8910 ay8910;
    cpudbg dbg;
    class crt crt;          // this is not a chip, but a cathode-ray-tube emulation
    uint8_t ram[8][0x4000];
    uint32_t rgba8_buffer[global_max_fb_width*global_max_fb_height]; // RGBA8 linear pixel buffer
};

} // namespace YAKC
