//------------------------------------------------------------------------------
//  breadboard.cc
//------------------------------------------------------------------------------
#include "yakc/util/core.h"
#define CHIPS_IMPL
#define CHIPS_ASSERT YAKC_ASSERT
#include "breadboard.h"

namespace YAKC {
uint8_t breadboard::ram[breadboard::num_ram_banks][breadboard::ram_bank_size];
uint8_t breadboard::random[breadboard::ram_bank_size];
uint8_t breadboard::junk[breadboard::ram_bank_size];
uint32_t breadboard::rgba8_buffer[global_max_fb_width*global_max_fb_height];
breadboard board;
}
