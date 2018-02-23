#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::AudioSource
    @brief implements a custom SoLoud audio source for sound generation
*/
#include "soloud.h"
#include "yakc/yakc.h"
#include <atomic>

namespace YAKC {

class AudioSource;

class AudioSourceInstance : public SoLoud::AudioSourceInstance {
public:
    AudioSourceInstance(AudioSource* aParent);
    virtual void getAudio(float* aBuffer, unsigned int aSamples);
    virtual bool hasEnded();

    AudioSource* parent;
};

class AudioSource : public SoLoud::AudioSource {
public:
    AudioSource();
    virtual SoLoud::AudioSourceInstance* createInstance();

    yakc* emu = nullptr;
    std::atomic<uint32_t> cpu_clock_speed;
    std::atomic<uint64_t> sample_cycle_count;
};

} // namespace YAKC

