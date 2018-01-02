#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::breadboard
    @brief houses all the common chips required by emulated systems
*/
#include "yakc/core/core.h"
#include "yakc/core/audiobuffer.h"
#include "yakc/chips/cpudbg.h"
//#include "yakc/chips/mc6845.h"
#include "yakc/peripherals/crt.h"
#include "chips/clk.h"
#include "chips/mem.h"
#include "chips/kbd.h"
#include "chips/beeper.h"
#include "chips/z80.h"
#include "chips/z80pio.h"
#include "chips/z80ctc.h"
#include "chips/ay38912.h"
#include "chips/m6502.h"
#include "chips/i8255.h"
#include "chips/mc6847.h"

namespace YAKC {

struct breadboard {
    int freq_hz = 0;
    mem_t mem;
    z80_t z80;
    z80pio_t z80pio;
    z80pio_t z80pio2;
    z80ctc_t z80ctc;
    ay38912_t ay38912;
    m6502_t m6502;
    i8255_t i8255;
    mc6847_t mc6847;
    beeper_t beeper;
    beeper_t beeper2;
    kbd_t kbd;
    /*
    class mc6845 mc6845;
    */
    class cpudbg dbg;
    class crt crt;          // this is not a chip, but a cathode-ray-tube emulation
    class audiobuffer audiobuffer;
    class audiobuffer audiobuffer2;
    static const int num_ram_banks = 8;
    static const int ram_bank_size = 0x4000;
    static uint8_t ram[num_ram_banks][ram_bank_size];
    static uint8_t random[ram_bank_size];  // a 16-kbyte bank filled with random numbers
    static uint8_t junk[ram_bank_size];    // a 16-kbyte page for junk writes
    static uint32_t rgba8_buffer[global_max_fb_width*global_max_fb_height]; // RGBA8 linear pixel buffer
};
extern breadboard board;

} // namespace YAKC
