#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio.h
    @brief implement audio playback callback
*/
#include "KC85Oryol.h"

class Audio {
public:
    /// setup audio playback
    void Setup(yakc::kc85& kc);
    /// shutdown audio playback
    void Discard();
    /// call per frame
    void Update();

private:
    /// convert 1.75MHz ticks to audio ticks, including a base delay
    unsigned int asAudioTicks(unsigned int z80Cycles) const;
    /// callback to start sound or change frequency
    static void cb_sound(void* userdata, unsigned int delay_cycles, int channel, int hz);
    /// callback to stop sound
    static void cb_stop(void* userdata, unsigned int delay_cycles, int channel);
    /// callback to change volume
    static void cb_volume(void* userdata, unsigned int delay_cycles, int vol);

    static const int baseDelayTicks = 44100 / 15;
    yakc::kc85* kc;
};