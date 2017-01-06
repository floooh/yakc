#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::speaker
    @brief a simple square-wave sound generator
*/
#include "yakc/devices/sound.h"

namespace YAKC {

class speaker : public sound {
public:
    /// initialize the sound generator
    void init(int cpu_khz, int sound_hz);
    /// reset the sound generator
    void reset();
    /// step the sound generator, call after each CPU instruction
    void step(int cpu_cycles);

    /// start playing a sound
    void start(int chn, int hz);
    /// stop playing a sound
    void stop(int chn);
    /// stop all channels
    void stop_all();

    static const int num_channels = 2;
    struct {
        uint16_t phase_add = 0;         // if 0, channel is disabled
        uint16_t phase_counter = 0;
    } channels[num_channels];
};

} // namespace YAKC
