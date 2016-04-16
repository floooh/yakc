//------------------------------------------------------------------------------
//  kc85_audiosource.cc
//------------------------------------------------------------------------------
#include "kc85_audiosource.h"
#include "Core/Log.h"

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

    uint32_t cpu_clock_speed;
    const uint64_t cpu_cycle_pos = this->parent->get_cpu_cycle_count_and_clock_speed(cpu_clock_speed)<<8;
    const uint32_t sample_rate = this->parent->sample_rate;
    const uint64_t cycles_per_sample = ((cpu_clock_speed<<8) / sample_rate);
    const int64_t max_cpu_audio_diff_cycles = (cpu_clock_speed / 5)<<8;   // allow roughly 0.2 sec divergence
    uint64_t cur_cycle_pos = this->parent->cur_sample_cycle_count<<8;

    // check if CPU time and audio time have diverged, correct if necessary
    int64_t diff = cpu_cycle_pos - cur_cycle_pos;
    if ((diff < -max_cpu_audio_diff_cycles) || (diff > max_cpu_audio_diff_cycles)) {
        this->parent->reset();
        Oryol::Log::Info("audio/cpu sync...\n");
    }

    kc85_audiosource::op cur_op;
    for (auto& chn : this->parent->channels) {
        cur_cycle_pos = this->parent->cur_sample_cycle_count<<8;
        chn.peek(cur_op);
        for (uint32_t i = 0; i < aSamples; i++) {
            chn.peek_next(cur_cycle_pos>>8, cur_op);
            if ((cur_op.vol > 0) && (cur_op.hz > 0)) {
                uint16_t phase_add = (0x10000 * cur_op.hz) / sample_rate;
                chn.phase_counter = (chn.phase_counter + phase_add) & 0xFFFF;
                *aBuffer++ = chn.phase_counter < 0x8000 ? 0.5f : -0.5f;
            }
            else {
                *aBuffer++ = 0.0f;
            }
            cur_cycle_pos += cycles_per_sample;
        }
    }
    // update sample-cycle-pos for next call with better precision
    this->parent->cur_sample_cycle_count = cur_cycle_pos>>8;
}

//------------------------------------------------------------------------------
bool
kc85_audioinstance::hasEnded() {
    return false;
}

