#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::sound
    @brief sound generator for simple beeper sound
*/
#include "yakc/core/core.h"
#include <atomic>

namespace YAKC {

class sound {
public:
    /// initialize the sound generator
    void init(int cpu_khz, int sound_hz);
    /// reset the sound generator
    void reset();
    /// step the sound generator, call after each CPU instruction
    void step(int cpu_cycles);
    /// fill sample buffer for external audio system (may be called from a thread!)
    void fill_samples(float* buffer, int num_samples);

    int sound_hz;               // playback frequency
    int sample_cycles;          // length of one audio sample in CPU cycles
    int sample_counter;         // need to generate new sample if counter goes below zero

    static const int precision = 8;
    static const int num_buffers = 32;
    static const int buf_size = 128;
    std::atomic<int> read_buffer = { 0 };
    std::atomic<int> write_buffer = { 0 };
    int write_pos = 0;
    float buf[num_buffers][buf_size];
};

} // namespace YAKC
