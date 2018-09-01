//------------------------------------------------------------------------------
//  breadboard.cc
//------------------------------------------------------------------------------
#include "yakc/util/core.h"
#include "breadboard.h"

namespace YAKC {
uint8_t breadboard::ram[breadboard::num_ram_banks][breadboard::ram_bank_size];
uint8_t breadboard::random[breadboard::ram_bank_size];
uint8_t breadboard::junk[breadboard::ram_bank_size];
uint32_t breadboard::rgba8_buffer[global_max_fb_width*global_max_fb_height];
breadboard board;

void breadboard::clear() {
    this->freq_hz = 0;
    this->mem = nullptr;
    this->z80 = nullptr;
    this->z80pio_1 = nullptr;
    this->z80pio_2 = nullptr;
    this->z80ctc = nullptr;
    this->ay38910 = nullptr;
    this->m6502 = nullptr;
    this->m6522 = nullptr;
    this->m6526_1 = nullptr;
    this->m6526_2 = nullptr;
    this->i8255 = nullptr;
    this->mc6847 = nullptr;
    this->mc6845 = nullptr;
    this->m6569 = nullptr;
    this->m6581 = nullptr;
    this->beeper_1 = nullptr;
    this->beeper_2 = nullptr;
    this->kbd = nullptr;
    this->crt = nullptr;
}

} // namespace YAKC
