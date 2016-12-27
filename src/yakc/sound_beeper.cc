//------------------------------------------------------------------------------
//  sound_beeper.cc
//------------------------------------------------------------------------------
#include "sound_beeper.h"

#include <stdio.h>

namespace YAKC {

//------------------------------------------------------------------------------
void
sound_beeper::init(int cpu_khz, int sound_hz) {
    sound::init(cpu_khz, sound_hz);
    this->reset();
}

//------------------------------------------------------------------------------
void
sound_beeper::reset() {
    this->state = false;
    this->value = 0.0f;
    this->write_sample_counter = 0;
    sound::reset();
}

//------------------------------------------------------------------------------
void
sound_beeper::write(bool b) {
    this->state = b;
}

//------------------------------------------------------------------------------
void
sound_beeper::step(int cpu_cycles) {
    // super-sample 4x
    this->sample_counter -= cpu_cycles * precision * super_sample;
    while (this->sample_counter <= 0) {
        // generate new super-sampled sample
        this->sample_counter += this->sample_cycles;
        if (this->state) {
            this->value += 0.5f;
        }

        // write a new sound sample?
        if (++this->write_sample_counter == super_sample) {
            this->write_sample_counter = 0;
            float* dst = &(this->buf[this->write_buffer][0]);
            dst[this->write_pos] = this->value / float(super_sample);
            this->value = 0.0f;
            this->write_pos = (this->write_pos + 1) & (buf_size-1);
            if (0 == this->write_pos) {
                this->write_buffer = (this->write_buffer + 1) & (num_buffers-1);
            }
        }
    }
}

} // namespace YAKC
