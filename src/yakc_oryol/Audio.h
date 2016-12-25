#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::Audio
    @brief implement audio playback callback
*/
#include "yakc/yakc.h"
#include "Core/Containers/Queue.h"
#include "AudioSource.h"
#include "soloud_biquadresonantfilter.h"

namespace YAKC {

class Audio {
public:
    /// setup audio playback
    void Setup(yakc* emu);
    /// shutdown audio playback
    void Discard();
    /// per-frame update
    void Update();
    /// get the current max processed audio sample count in number of CPU cycles
    uint64_t GetProcessedCycles() const;

    static SoLoud::Soloud* soloud;
    static int soloud_open_count;
    yakc* emu = nullptr;
    SoLoud::BiquadResonantFilter filter;
    AudioSource audioSource;
    int audioHandle = 0;
};

} // namespace YAKC
