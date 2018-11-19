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
#include "chips/m6569.h"
#include "chips/m6526.h"
#include "chips/m6581.h"
#include "chips/fdd.h"
#include "chips/fdd_cpc.h"
#include "chips/upd765.h"

namespace YAKC {

struct breadboard {
    // clear all pointers
    void clear();

    int freq_hz = 0;
    mem_t* mem = nullptr;
    z80_t* z80 = nullptr;
    z80pio_t* z80pio_1 = nullptr;
    z80pio_t* z80pio_2 = nullptr;
    z80ctc_t* z80ctc = nullptr;
    ay38910_t* ay38910 = nullptr;
    m6502_t* m6502 = nullptr;
    m6522_t* m6522 = nullptr;
    m6526_t* m6526_1 = nullptr;
    m6526_t* m6526_2 = nullptr;
    i8255_t* i8255 = nullptr;
    mc6847_t* mc6847 = nullptr;
    mc6845_t* mc6845 = nullptr;
    m6569_t* m6569 = nullptr;
    m6581_t*  m6581 = nullptr;
    beeper_t* beeper_1 = nullptr;
    beeper_t* beeper_2 = nullptr;
    kbd_t* kbd = nullptr;
    crt_t* crt = nullptr;
    class debugger dbg;
    int audio_sample_rate = 44100;
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
