//------------------------------------------------------------------------------
//  sound_ay8910.cc
//------------------------------------------------------------------------------
#include "sound_ay8910.h"

namespace YAKC {

ubyte ay8910_masks[sound_ay8910::NUM_REGS] = {
    0xFF,       // TONE_PERIOD_A_FINE
    0x0F,       // TONE_PERIOD_A_COARSE
    0xFF,       // TONE_PERIOD_B_FINE
    0x0F,       // TONE_PERIOD_B_COARSE
    0xFF,       // TONE_PERIOD_C_FINE
    0x0F,       // TONE_PERIOD_C_COARSE
    0x1F,       // NOISE_PERIOD
    0xFF,       // ENABLE
    0x1F,       // AMP_A
    0x1F,       // AMP_B
    0x1F,       // AMP_C
    0xFF,       // ENV_PERIOD_FINE
    0xFF,       // ENV_PERIOD_COARSE
    0x0F,       // ENV_SHAPE_CYCLE
    0xFF,       // IO_PORT_A
    0xFF,       // IO_PORT_B
};

//------------------------------------------------------------------------------
void
sound_ay8910::init(int cpu_khz, int sound_hz) {
    sound::init(cpu_khz, sound_hz);
    this->reset();
}

//------------------------------------------------------------------------------
void
sound_ay8910::reset() {
    for (int i = 0; i < NUM_REGS; i++) {
        this->regs[i] = 0;
    }
}

//------------------------------------------------------------------------------
void
sound_ay8910::step(int cpu_cycles) {
    // FIXME!
    sound::step(cpu_cycles);
}

//------------------------------------------------------------------------------
void
sound_ay8910::write(ubyte reg, ubyte val) {
    if (reg < NUM_REGS) {
        this->regs[reg] = val & ay8910_masks[reg];
    }
}

//------------------------------------------------------------------------------
ubyte
sound_ay8910::read(ubyte reg) const {
    if (reg < NUM_REGS) {
        return this->regs[reg];
    }
    else {
        return 0;
    }
}

} // namespace YAKC


