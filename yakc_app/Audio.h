#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio.h
    @brief implement audio playback callback
*/
#include "KC85Oryol.h"
#include "Sound/Sound.h"

class Audio {
public:
    /// setup audio playback
    void Setup();
    /// shutdown audio playback
    void Discard();
    /// call per frame to update audio playback parameters
    void Update(yakc::kc85& kc);

    /// compute CTC channel frequency
    int channelFreq(const yakc::kc85& kc, yakc::z80ctc::channel c);

    /// channel 0 active
    bool chn0Active = false;
    /// channel 1 active
    bool chn1Active = false;
    /// current frequency on CTC0
    int curFreq0 = 0;
    /// current frequency on CTC1
    int curFreq1 = 0;
    /// current volume (defined in PIO-B)
    int curVolume = 0;
};