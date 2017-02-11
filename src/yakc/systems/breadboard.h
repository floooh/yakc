#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::breadboard
    @brief houses all the common chips required by emulated systems
*/
#include "yakc/core/core.h"
#include "yakc/core/clock.h"
#include "yakc/chips/z80.h"
#include "yakc/chips/mos6502.h"
#include "yakc/chips/cpudbg.h"
#include "yakc/chips/z80pio.h"
#include "yakc/chips/z80ctc.h"
#include "yakc/chips/i8255.h"
#include "yakc/chips/mos6522.h"
#include "yakc/chips/mc6845.h"
#include "yakc/chips/mc6847.h"
#include "yakc/chips/ay8910.h"
#include "yakc/peripherals/beeper.h"
#include "yakc/peripherals/speaker.h"
#include "yakc/peripherals/crt.h"

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
    class mos6522 mos6522;
    class mc6845 mc6845;
    class mc6847 mc6847;
    class beeper beeper;
    class speaker speaker;
    class ay8910 ay8910;
    cpudbg dbg;
    class crt crt;          // this is not a chip, but a cathode-ray-tube emulation
    static const int num_ram_banks = 8;
    static const int ram_bank_size = 0x4000;
    uint8_t ram[num_ram_banks][ram_bank_size];
    uint8_t random[ram_bank_size]; // a 16-kbyte bank filled with random numbers
    uint32_t rgba8_buffer[global_max_fb_width*global_max_fb_height]; // RGBA8 linear pixel buffer
};

} // namespace YAKC
