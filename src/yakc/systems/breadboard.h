#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::breadboard
    @brief houses all the common chips required by emulated systems
*/
#include "yakc/core/core.h"
//#include "yakc/chips/mos6502.h"
#include "yakc/chips/cpudbg.h"
//#include "yakc/chips/i8255.h"
//#include "yakc/chips/mos6522.h"
//#include "yakc/chips/mc6845.h"
//#include "yakc/chips/mc6847.h"
//#include "yakc/chips/ay8910.h"
//#include "yakc/peripherals/beeper.h"
//#include "yakc/peripherals/speaker.h"
#include "yakc/peripherals/crt.h"
#include "chips/clk.h"
#include "chips/mem.h"
#include "chips/kbd.h"
#include "chips/z80.h"
#include "chips/z80pio.h"
#include "chips/z80ctc.h"

namespace YAKC {

struct breadboard {
    int freq_khz = 0;
    mem_t mem;
    z80_t z80;
    z80pio_t z80pio;
    z80pio_t z80pio2;
    z80ctc_t z80ctc;
    /*
    class mos6502 mos6502;
    class i8255 i8255;
    class mos6522 mos6522;
    class mc6845 mc6845;
    class mc6847 mc6847;
    class ay8910 ay8910;
    class beeper beeper;
    class speaker speaker;
    */
    kbd_t kbd;
    class cpudbg dbg;
    class crt crt;          // this is not a chip, but a cathode-ray-tube emulation
    static const int num_ram_banks = 8;
    static const int ram_bank_size = 0x4000;
    static uint8_t ram[num_ram_banks][ram_bank_size];
    static uint8_t random[ram_bank_size];  // a 16-kbyte bank filled with random numbers
    static uint8_t junk[ram_bank_size];    // a 16-kbyte page for junk writes
    static uint32_t rgba8_buffer[global_max_fb_width*global_max_fb_height]; // RGBA8 linear pixel buffer
};
extern breadboard board;

} // namespace YAKC
