//------------------------------------------------------------------------------
//  ay8910.cc
//
//  NOTE: hardware envelopes are implemented but not tested, I haven't
//  found CPC games which use hw envelopes!
//------------------------------------------------------------------------------
#include "ay8910.h"

namespace YAKC {

uint8_t ay8910_masks[ay8910::NUM_REGS] = {
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
ay8910::init(int cpu_khz, int ay_khz, int sound_hz) {
    // NOTE: cpu_khz should be a multiple of ay_khz
    sound::init(cpu_khz, sound_hz);
    this->tone_update.init((cpu_khz * 8) / ay_khz);
    this->reset();
}

//------------------------------------------------------------------------------
void
ay8910::reset() {
    this->sel = 0;
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
    this->env_period = 0;
    this->env_count = 0;
    this->env_cycle_count = 0;
    this->env_volume = 0;
    this->env_volume_add = 0;
}

//------------------------------------------------------------------------------
void
ay8910::step(int cpu_cycles) {

    // update counters
    this->tone_update.update(cpu_cycles);
    while (this->tone_update.step()) {
        for (int i = 0; i < num_channels; i++) {
            auto& chn = this->channels[i];
            chn.count++;
            if (chn.count >= chn.period) {
                chn.count = 0;
                chn.bit ^= 1;
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

        // update the envelope generator
        this->env_count++;
        if (this->env_count >= this->env_period) {
            this->env_count = 0;

            // update the envelope volume
            this->env_volume = (this->env_volume + this->env_volume_add) & 0xF;

            // check if an envelope cycle is complete
            // http://dev-docs.atariforge.org/files/GI_AY-3-8910_Feb-1979.pdf
            if (0 != this->env_volume_add) {
                this->env_cycle_count++;
                if (this->env_cycle_count >= 16) {
                    this->env_cycle_count = 0;
                    const uint8_t env_ctrl = this->regs[ENV_SHAPE_CYCLE];

                    // HOLD
                    if (env_ctrl & (1<<0)) {
                        if (env_ctrl & (1<<1)) {
                            // if both hold and alternate are enabled,
                            // set the volume to initial value
                            this->env_volume = (this->env_volume_add>0) ? 0 : 15;
                        }
                        else {
                            this->env_volume = (this->env_volume_add>0) ? 15 : 0;
                        }
                        this->env_volume_add = 0;
                    }

                    // ALTERNATE
                    if (env_ctrl & (1<<1)) {
                        this->env_volume_add = -this->env_volume_add;
                    }

                    // CONTINUE not set?
                    if (0 == (env_ctrl & (1<<3))) {
                        this->env_volume_add = 0;
                        this->env_volume = 0;
                    }
                }
            }
        }
    }

    // generate new sample?
    this->sample_counter.update(cpu_cycles * precision);
    while (this->sample_counter.step()) {
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
                vol = ay8910_volumes[this->env_volume];
            }
            int vol_enable = (chn.bit|chn.tone_disable) & ((this->noise_rng&1)|chn.noise_disable);
            s += vol_enable ? vol : -vol;
        }
        float* dst = &(this->buf[this->write_buffer][0]);
        dst[this->write_pos] = s * 0.33f;
        this->write_pos = (this->write_pos + 1) & (chunk_size-1);
        if (0 == this->write_pos) {
            this->write_buffer = (this->write_buffer + 1) & (num_chunks-1);
        }
    }
}

//------------------------------------------------------------------------------
void
ay8910::write(uint8_t val) {
    if (this->sel < NUM_REGS) {
        this->regs[this->sel] = val & ay8910_masks[this->sel];

        // update computed values
        for (int i = 0; i < num_channels; i++) {
            auto& chn = this->channels[i];
            chn.period = ((this->regs[2*i+1]<<8) | this->regs[2*i]) & 0x0FFF;
            chn.tone_disable  = ((this->regs[ENABLE]>>i) & 1);
            chn.noise_disable = ((this->regs[ENABLE]>>(i+3)) & 1);
        }
        this->noise_period = this->regs[NOISE_PERIOD];
        this->env_period = 16 * (((this->regs[ENV_PERIOD_COARSE]<<8) | this->regs[ENV_PERIOD_FINE]) & 0xFFFF);
        if (ENV_SHAPE_CYCLE == this->sel) {
            // Bit 0  Hold        (1=stop envelope past first cycle)
            // Bit 1  Alternate   (1=reverse direction at end of each cycle)
            // Bit 2  Attack      (1=initial direction increase)
            // Bit 3  Continue    (0=same as if Bit0=1 and Bit1=Bit2)
            this->env_cycle_count = 0;
            this->env_volume_add = (val & (1<<2)) ? +1 : -1;
            this->env_volume = (this->env_volume_add > 0) ? 0 : 15;
        }
    }
}

//------------------------------------------------------------------------------
uint8_t
ay8910::read() const {
    if (this->sel < NUM_REGS) {
        return this->regs[this->sel];
    }
    else {
        return 0;
    }
}

} // namespace YAKC


