#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::sound_beeper.cc
    @brief simple CPU-driven beeper sound
*/
#include "yakc/sound.h"

namespace YAKC {

class sound_beeper : public sound {
public:
    /// initialize the sound generator
    void init(int cpu_khz, int sound_hz);
    /// reset the sound generator
    void reset();
    /// step the sound generator, call after each CPU instruction
    void step(int cpu_cycles);
    /// set current beeper bit state on/off
    void write(bool b);

    static const int super_sample = 4;
    bool state = false;     // current beeper on/off state
    float value = 0.0f;
    int write_sample_counter = 0;
};

} // namespace YAKC
