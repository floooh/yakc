//------------------------------------------------------------------------------
//  AudioSource.cc
//------------------------------------------------------------------------------
#include "AudioSource.h"
#include "Core/Log.h"

namespace YAKC {

//------------------------------------------------------------------------------
AudioSource::AudioSource() {
    this->mChannels = 1;
    this->mBaseSamplerate = SOUND_SAMPLE_RATE;
    this->sample_cycle_count = 0;
}

//------------------------------------------------------------------------------
SoLoud::AudioSourceInstance*
AudioSource::createInstance() {
    return new AudioSourceInstance(this);
}

//------------------------------------------------------------------------------
AudioSourceInstance::AudioSourceInstance(AudioSource* aParent) :
parent(aParent) {
    // empty
}

//------------------------------------------------------------------------------
void
AudioSourceInstance::getAudio(float* aBuffer, unsigned int aSamples) {
    // NOTE: this may be called from a thread, depending on
    // SoLoud backend and platform
    const uint64_t cpu_clock_speed = this->parent->cpu_clock_speed;
    const uint64_t sample_rate = SOUND_SAMPLE_RATE;
    this->parent->emu->fill_sound_samples(aBuffer, aSamples);
    this->parent->sample_cycle_count += (aSamples * cpu_clock_speed) / sample_rate;
}

//------------------------------------------------------------------------------
bool
AudioSourceInstance::hasEnded() {
    return false;
}

} // namespace YAKC


