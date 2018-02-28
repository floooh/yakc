#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::breadboard
    @brief houses all the common chips required by emulated systems
*/
#include "yakc/util/core.h"
#include "yakc/util/audiobuffer.h"
#include "yakc/util/debugger.h"
#include "chips/clk.h"
#include "chips/mem.h"
#include "chips/kbd.h"
#include "chips/crt.h"
#include "chips/beeper.h"
#include "chips/z80.h"
#include "chips/z80pio.h"
#include "chips/z80ctc.h"
#include "chips/ay38910.h"
#include "chips/m6502.h"
#include "chips/m6522.h"
#include "chips/i8255.h"
#include "chips/mc6847.h"
#include "chips/mc6845.h"
#include "chips/m6567.h"
#include "chips/m6526.h"

namespace YAKC {

struct breadboard {
    int freq_hz = 0;
    mem_t mem;
    mem_t mem2;
    z80_t z80;
    z80pio_t z80pio_1;
    z80pio_t z80pio_2;
    z80ctc_t z80ctc;
    ay38910_t ay38910;
    m6502_t m6502;
    m6522_t m6522;
    m6526_t m6526_1;
    m6526_t m6526_2;
    i8255_t i8255;
    mc6847_t mc6847;
    mc6845_t mc6845;
    m6567_t m6567;
    beeper_t beeper_1;
    beeper_t beeper_2;
    kbd_t kbd;
    crt_t crt;
    class debugger dbg;
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
