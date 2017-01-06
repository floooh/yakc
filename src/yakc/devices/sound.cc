//------------------------------------------------------------------------------
//  sound.cc
//------------------------------------------------------------------------------
#include "sound.h"

#include <stdio.h>

namespace YAKC {

//------------------------------------------------------------------------------
void
sound::init(int cpu_khz_, int sound_hz_) {
    YAKC_ASSERT((cpu_khz_ > 0) && (sound_hz_ > 0));
    this->sound_hz = sound_hz_;
    this->read_buffer = 0;
    this->write_buffer = 0;
    this->write_pos = 0;
    // improve sample_cycle counter precision 8x
    this->sample_cycles = (cpu_khz_ * 1000 * precision) / sound_hz_;
    YAKC_ASSERT(this->sample_cycles > 0);
    this->sample_counter = this->sample_cycles;
    clear(this->buf, sizeof(this->buf));
}

//------------------------------------------------------------------------------
void
sound::reset() {
    this->sample_counter = this->sample_cycles;
    this->read_buffer = 0;
    this->write_buffer = 0;
    this->write_pos = 0;
    clear(this->buf, sizeof(this->buf));
}

//------------------------------------------------------------------------------
void
sound::step(int cpu_cycles) {
    this->sample_counter -= cpu_cycles * precision;
    while (this->sample_counter <= 0) {
        this->sample_counter += this->sample_cycles;
        float* dst = &(this->buf[this->write_buffer][0]);
        dst[this->write_pos] = 0.0f;
        this->write_pos = (this->write_pos + 1) & (buf_size-1);
        if (0 == this->write_pos) {
            this->write_buffer = (this->write_buffer + 1) & (num_buffers-1);
        }
    }
}

//------------------------------------------------------------------------------
void
sound::fill_samples(float* buffer, int num_samples) {
    YAKC_ASSERT((num_samples % buf_size) == 0);
    YAKC_ASSERT((num_samples >= buf_size));
    float* dst_ptr = buffer;
    for (int i = 0; i < (num_samples / buf_size); i++) {
        if (this->read_buffer == this->write_buffer) {
            // CPU was falling behind, add a block of silence
            clear(dst_ptr, buf_size * sizeof(float));
        }
        else {
            // copy valid sound data
            float* src_ptr = &(this->buf[this->read_buffer][0]);
            memcpy(dst_ptr, src_ptr, buf_size * sizeof(float));
            this->read_buffer = (this->read_buffer + 1) & (num_buffers-1);
        }
        dst_ptr += buf_size;
    }
}

} // namespace YAKC
