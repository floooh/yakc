#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio.h
    @brief implement audio playback callback
*/
#include "KC85Oryol.h"
#include "Core/Containers/Queue.h"
#include "kc85_audiosource.h"
#include "soloud_biquadresonantfilter.h"

class Audio {
public:
    /// setup audio playback
    void Setup(yakc::kc85& kc);
    /// shutdown audio playback
    void Discard();
    /// update the CPU cycle count, this is used to sync CPU time and audio time
    void UpdateCpuCycles(uint32_t cpu_clock_speed, uint32_t cpu_cycles);

    /// convert 1.75MHz ticks to audio ticks, including a base delay
    unsigned int asAudioTicks(unsigned int z80Cycles) const;
    /// callback to start sound or change frequency
    static void cb_sound(void* userdata, uint64_t cycle_count, int channel, int hz);
    /// callback to stop sound
    static void cb_stop(void* userdata, uint64_t cycle_count, int channel);
    /// callback to change volume
    static void cb_volume(void* userdata, uint64_t cycle_count, int vol);

    yakc::kc85* kc = nullptr;
    SoLoud::Soloud soloud;
    SoLoud::BiquadResonantFilter filter;
    kc85_audiosource audioSource;
    int audioHandle = 0;
};