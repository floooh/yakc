//------------------------------------------------------------------------------
//  sound.cc
//------------------------------------------------------------------------------
#include "sound.h"

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
    this->sample_counter.init((cpu_khz_ * 1000 * precision) / sound_hz_);
    YAKC_ASSERT(this->sample_counter.period > 0);
    clear(this->buf, sizeof(this->buf));
}

//------------------------------------------------------------------------------
void
sound::reset() {
    this->sample_counter.reset();
    this->read_buffer = 0;
    this->write_buffer = 0;
    this->write_pos = 0;
    clear(this->buf, sizeof(this->buf));
}

//------------------------------------------------------------------------------
void
sound::step(int cpu_cycles) {
    this->sample_counter.update(cpu_cycles * precision);
    while(sample_counter.step()) {
        float* dst = &(this->buf[this->write_buffer][0]);
        dst[this->write_pos] = 0.0f;
        this->write_pos = (this->write_pos + 1) & (chunk_size-1);
        if (0 == this->write_pos) {
            this->write_buffer = (this->write_buffer + 1) & (num_chunks-1);
        }
    }
}

//------------------------------------------------------------------------------
void
sound::fill_samples(float* buffer, int num_samples, bool mix) {
    YAKC_ASSERT((num_samples % chunk_size) == 0);
    YAKC_ASSERT((num_samples >= chunk_size));
    float* dst_ptr = buffer;
    for (int chunk_index = 0; chunk_index < (num_samples / chunk_size); chunk_index++) {
        if (this->read_buffer == this->write_buffer) {
            // CPU was falling behind, add a block of silence
            clear(dst_ptr, chunk_size * sizeof(float));
        }
        else if (mix) {
            float* src_ptr = &(this->buf[this->read_buffer][0]);
            for (int i = 0; i < chunk_size; i++) {
                dst_ptr[i] += src_ptr[i];
            }
            this->read_buffer = (this->read_buffer + 1) & (num_chunks-1);
        }
        else {
            // copy valid sound data
            float* src_ptr = &(this->buf[this->read_buffer][0]);
            memcpy(dst_ptr, src_ptr, chunk_size * sizeof(float));
            this->read_buffer = (this->read_buffer + 1) & (num_chunks-1);
        }
        dst_ptr += chunk_size;
    }
}

} // namespace YAKC
