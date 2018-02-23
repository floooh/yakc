//------------------------------------------------------------------------------
/**
    @class YAKC::audiobuffer
    @brief buffer samples from emulation to be consumed by audio API

    This is a simple ringbuffer which buffers audio samples as they
    are coming out of emulation until they are consumed by the
    external audio playback callback.
*/
#include "yakc/util/core.h"
#include <atomic>

namespace YAKC {

class audiobuffer {
public:
    /// initialize / reset the audiobuffer
    void init();
    /// push a new generated audio sample into the buffer
    void write(float sample);
    /// read samples into external audio system buffer (may be called from a thread)
    void read(float* buffer, int num_samples, bool mix=false);

    static const int num_chunks = 32;
    static const int chunk_size = 128;
    std::atomic<int> read_chunk = { 0 };
    std::atomic<int> write_chunk = { 0 };
    float last_sample = 0.0f;
    int write_pos = 0;
    float buf[num_chunks][chunk_size];
};

//------------------------------------------------------------------------------
inline void audiobuffer::write(float sample) {
    this->last_sample = sample;
    float* dst = &(this->buf[this->write_chunk][0]);
    dst[this->write_pos] = sample;
    this->write_pos = (this->write_pos + 1) & (chunk_size - 1);
    if (0 == this->write_pos) {
        this->write_chunk = (this->write_chunk + 1) & (num_chunks - 1);
    }
}

} // namespace YAKC
