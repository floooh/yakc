//------------------------------------------------------------------------------
//  kc85_audiosource.cc
//------------------------------------------------------------------------------
#include "kc85_audiosource.h"
#include "Core/Log.h"

namespace YAKC {

//------------------------------------------------------------------------------
kc85_audiosource::kc85_audiosource() {
    this->mChannels = 2;
}

//------------------------------------------------------------------------------
SoLoud::AudioSourceInstance*
kc85_audiosource::createInstance() {
    return new kc85_audioinstance(this);
}

//------------------------------------------------------------------------------
kc85_audioinstance::kc85_audioinstance(kc85_audiosource* aParent) :
parent(aParent) {
    // empty
}

//------------------------------------------------------------------------------
void
kc85_audioinstance::getAudio(float* aBuffer, unsigned int aSamples) {
    // NOTE: this may be called from a thread, depending on
    // SoLoud backend and platform

    const int precision = 6;

    const uint64_t cpu_clock_speed = this->parent->cpu_clock_speed;
    const uint64_t sample_rate = this->parent->sample_rate;
    const uint64_t cycles_per_sample = ((cpu_clock_speed<<precision) / sample_rate);
    kc85_audiosource::op cur_op;
    uint64_t cur_cycle_count = 0;
    for (auto& chn : this->parent->channels) {
        cur_cycle_count = this->parent->sample_cycle_count<<precision;
        chn.peek(cur_op);
        for (uint32_t i = 0; i < aSamples; i++) {
            chn.peek_next(cur_cycle_count>>precision, cur_op);
            if ((cur_op.vol > 0) && (cur_op.hz > 0)) {
                uint16_t phase_add = (0x10000 * cur_op.hz) / sample_rate;
                chn.phase_counter = (chn.phase_counter + phase_add) & 0xFFFF;
                *aBuffer++ = chn.phase_counter < 0x8000 ? 0.5f : -0.5f;
            }
            else {
                *aBuffer++ = 0.0f;
            }
            cur_cycle_count += cycles_per_sample;
        }
    }
    this->parent->sample_cycle_count = cur_cycle_count>>precision;
}

//------------------------------------------------------------------------------
bool
kc85_audioinstance::hasEnded() {
    return false;
}

} // namespace YAKC


