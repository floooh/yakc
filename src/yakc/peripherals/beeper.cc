//------------------------------------------------------------------------------
//  beeper.cc
//------------------------------------------------------------------------------
#include "beeper.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
beeper::init(int cpu_khz, int sound_hz) {
    sound::init(cpu_khz, sound_hz);
    this->reset();
}

//------------------------------------------------------------------------------
void
beeper::reset() {
    this->state = false;
    this->value = 0.0f;
    this->write_sample_counter = 0;
    sound::reset();
}

//------------------------------------------------------------------------------
void
beeper::write(bool b) {
    this->state = b;
}

//------------------------------------------------------------------------------
void
beeper::step(int cpu_cycles) {
    // super-sample 4x
    this->sample_counter.update(cpu_cycles * precision * super_sample);
    while (this->sample_counter.step()) {
        // generate new super-sampled sample
        if (this->state) {
            this->value += 0.5f;
        }

        // write a new sound sample?
        if (++this->write_sample_counter == super_sample) {
            this->write_sample_counter = 0;
            float* dst = &(this->buf[this->write_buffer][0]);
            dst[this->write_pos] = this->value / float(super_sample);
            this->value = 0.0f;
            this->write_pos = (this->write_pos + 1) & (chunk_size-1);
            if (0 == this->write_pos) {
                this->write_buffer = (this->write_buffer + 1) & (num_chunks-1);
            }
        }
    }
}

} // namespace YAKC
