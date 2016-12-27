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

// volume table from: https://github.com/true-grue/ayumi/blob/master/ayumi.c
float ay8910_volumes[16] = {
  0.0f,
  0.00999465934234f,
  0.0144502937362f,
  0.0210574502174f,
  0.0307011520562f,
  0.0455481803616f,
  0.0644998855573f,
  0.107362478065f,
  0.126588845655f,
  0.20498970016f,
  0.292210269322f,
  0.372838941024f,
  0.492530708782f,
  0.635324635691f,
  0.805584802014f,
  1.0f
};

//------------------------------------------------------------------------------
void
sound_ay8910::init(int cpu_khz, int ay_khz, int sound_hz) {
    // NOTE: cpu_khz should be a multiple of ay_khz
    sound::init(cpu_khz, sound_hz);
    this->freq_length = (cpu_khz * 8) / ay_khz;
    this->freq_count = this->freq_length;
    this->reset();
}

//------------------------------------------------------------------------------
void
sound_ay8910::reset() {
    for (int i = 0; i < NUM_REGS; i++) {
        this->regs[i] = 0;
    }
    for (auto& chn : this->channels) {
        chn = channel_t();
    }
    this->noise_period = 0;
    this->noise_count = 0;
    this->noise_bit = 0;
    this->noise_rng = 1;
}

//------------------------------------------------------------------------------
void
sound_ay8910::step(int cpu_cycles) {

    // update tone and noise frequency counters
    this->freq_count -= cpu_cycles;
    while (this->freq_count <= 0) {
        this->freq_count += this->freq_length;
        for (int i = 0; i < num_channels; i++) {
            auto& chn = this->channels[i];
            if ((this->regs[AMP_A+i] & 0x0F) > 0) {
                chn.count++;
                if (chn.count >= chn.period) {
                    chn.count = 0;
                    chn.bit ^= 1;
                }
            }
            else {
                chn.bit = 0;
            }
        }
        this->noise_count++;
        if (this->noise_count >= this->noise_period) {
            this->noise_count = 0;
            this->noise_bit ^= 1;
            if (this->noise_bit) {
                // random number generator from MAME:
                // https://github.com/mamedev/mame/blob/master/src/devices/sound/ay8910.cpp        }
				// The Random Number Generator of the 8910 is a 17-bit shift
				// register. The input to the shift register is bit0 XOR bit3
				// (bit0 is the output). This was verified on AY-3-8910 and YM2149 chips.
                this->noise_rng ^= (((this->noise_rng & 1) ^ ((this->noise_rng >> 3) & 1)) << 17);
				this->noise_rng >>= 1;
            }
        }
    }

    // generate new sample?
    this->sample_counter -= cpu_cycles * precision;
    while (this->sample_counter <= 0) {
        this->sample_counter += this->sample_cycles;
        float vol = 0.0f;
        float s = 0.0f;
        for (int i = 0; i < num_channels; i++) {
            auto& chn = this->channels[i];
            if (0 == (this->regs[AMP_A+i] & (1<<4))) {
                // fixed amplitude
                vol = ay8910_volumes[this->regs[AMP_A+i] & 0x0F];
            }
            else {
                // amplitude from envelope generator
                vol = 0.75f;
            }
            int vol_enable = (chn.bit|chn.tone_disable) & ((this->noise_rng&1)|chn.noise_disable);
            s += vol_enable ? vol : -vol;
        }
        float* dst = &(this->buf[this->write_buffer][0]);
        dst[this->write_pos] = s * 0.33f;
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
        for (int i = 0; i < num_channels; i++) {
            auto& chn = this->channels[i];
            chn.period = ((this->regs[2*i+1]<<8) | this->regs[2*i]) & 0x0FFF;
            if (0 == chn.period) {
                chn.period = 1;
            }
            chn.tone_disable  = ((this->regs[ENABLE]>>i) & 1);
            chn.noise_disable = ((this->regs[ENABLE]>>(i+3)) & 1);
        }
        this->noise_period = this->regs[NOISE_PERIOD];
        if (0 == this->noise_period) {
            this->noise_period = 1;
        }
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


