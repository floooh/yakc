//------------------------------------------------------------------------------
//  sound_speaker.cc
//------------------------------------------------------------------------------
#include "sound_speaker.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
sound_speaker::init(int cpu_khz, int sound_hz) {
    sound::init(cpu_khz, sound_hz);
    for (auto& chn : this->channels) {
        chn.phase_add = 0;
        chn.phase_counter = 0;
    }
}

//------------------------------------------------------------------------------
void
sound_speaker::reset() {
    sound::reset();
    for (auto& chn : this->channels) {
        chn.phase_add = 0;
        chn.phase_counter = 0;
    }
}

//------------------------------------------------------------------------------
void
sound_speaker::start(int chn, int hz) {
    YAKC_ASSERT((chn >= 0) && (chn < num_channels));
    this->channels[chn].phase_add = uint16_t((0x10000 * hz) / this->sound_hz);
}

//------------------------------------------------------------------------------
void
sound_speaker::stop(int chn) {
    YAKC_ASSERT((chn >= 0) && (chn < num_channels));
    this->channels[chn].phase_add = 0;
}

//------------------------------------------------------------------------------
void
sound_speaker::stop_all() {
    for (auto& chn : this->channels) {
        chn.phase_add = 0;
    }
}

//------------------------------------------------------------------------------
void
sound_speaker::step(int cpu_cycles) {
    this->sample_counter -= cpu_cycles * precision;
    while (this->sample_counter < 0) {
        // generate new sample
        this->sample_counter += this->sample_cycles;
        float s = 0.0f;
        for (int i = 0; i < 2; i++) {
            auto& chn = this->channels[i];
            if (chn.phase_add > 0) {
                chn.phase_counter += chn.phase_add;
                s += (chn.phase_counter < 0x8000) ? 0.5f : -0.5f;
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

} // namespace YAKC
