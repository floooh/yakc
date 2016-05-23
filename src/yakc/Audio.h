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
    void Setup(const yakc::clock& clk);
    /// shutdown audio playback
    void Discard();
    /// per-frame update
    void Update(const yakc::clock& clk);
    /// get the current max processed audio sample count in number of CPU cycles
    uint64_t GetProcessedCycles() const;

    /// callback to start sound or change frequency
    static void cb_sound(void* userdata, uint64_t cycle_count, int channel, int hz);
    /// callback to stop sound
    static void cb_stop(void* userdata, uint64_t cycle_count, int channel);
    /// callback to change volume
    static void cb_volume(void* userdata, uint64_t cycle_count, int vol);

    SoLoud::Soloud soloud;
    SoLoud::BiquadResonantFilter filter;
    kc85_audiosource audioSource;
    int audioHandle = 0;
};