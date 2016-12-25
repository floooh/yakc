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
sound_ay8910::init(int cpu_khz, int ay_khz, int sound_hz) {
    sound::init(cpu_khz, sound_hz);
    this->tone_length = (cpu_khz / ay_khz) * 16;
    this->tone_count = this->tone_length;
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

    // update tone and noise frequency counters
    this->tone_count -= cpu_cycles;
    while (this->tone_count <= 0) {
        this->tone_count += this->tone_length;
        for (auto& chn : this->channels) {
            chn.count++;
            if (chn.count >= chn.period) {
                chn.count = 0;
                chn.bit = !chn.bit;
            }
        }
    }

    // generate new sample?
    ubyte tone_enable = this->regs[ENABLE] & 0x7;
    this->sample_counter -= cpu_cycles * precision;
    while (this->sample_counter < 0) {
        this->sample_counter += this->sample_cycles;
        float s = 0.0f;
        for (int i = 0; i < num_channels; i++) {
            if (0 == (tone_enable & (1<<i))) {
                auto& chn = this->channels[i];
                s += chn.bit ? 0.5f : -0.5f;
            }
        }
        float* dst = &(this->buf[this->write_buffer][0]);
        dst[this->write_pos] = s;
        this->write_pos = (this->write_pos + 1) & (buf_size-1);
        if (0 == this->write_pos) {
            this->write_buffer = (this->write_buffer + 1) & (num_buffers-1);
        }
    }
}

//------------------------------------------------------------------------------
void
sound_ay8910::write(ubyte reg, ubyte val) {
    if (reg < NUM_REGS) {
        this->regs[reg] = val & ay8910_masks[reg];

        // update computed values
        this->channels[0].period = ((this->regs[TONE_PERIOD_A_COARSE]<<8)|this->regs[TONE_PERIOD_A_FINE]) & 0x0FFF;
        this->channels[1].period = ((this->regs[TONE_PERIOD_B_COARSE]<<8)|this->regs[TONE_PERIOD_B_FINE]) & 0x0FFF;
        this->channels[2].period = ((this->regs[TONE_PERIOD_C_COARSE]<<8)|this->regs[TONE_PERIOD_C_FINE]) & 0x0FFF;
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


