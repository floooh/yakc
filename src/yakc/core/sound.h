#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::sound
    @brief base class for sound generators
*/
#include "yakc/core/core.h"
#include "yakc/core/counter.h"
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
    void fill_samples(float* buffer, int num_samples, bool mix=false);

    int sound_hz;               // playback frequency
    counter sample_counter;     // triggers when new sample needs to be generated

    static const int precision = 8;
    static const int num_chunks = 32;
    static const int chunk_size = 128;
    std::atomic<int> read_buffer = { 0 };
    std::atomic<int> write_buffer = { 0 };
    int write_pos = 0;
    float buf[num_chunks][chunk_size];
};

} // namespace YAKC
