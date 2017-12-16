//------------------------------------------------------------------------------
//  audiobuffer.cc
//------------------------------------------------------------------------------
#include "audiobuffer.h"

namespace YAKC {

//------------------------------------------------------------------------------
void audiobuffer::init() {
    this->read_chunk = 0;
    this->write_chunk = 0;
    this->write_pos = 0;
    clear(this->buf, sizeof(this->buf));
}

//------------------------------------------------------------------------------
void audiobuffer::read(float* buffer, int num_samples, bool mix) {
    YAKC_ASSERT((num_samples % chunk_size) == 0);
    YAKC_ASSERT((num_samples >= chunk_size));
    float* dst_ptr = buffer;
    for (int chunk_index = 0; chunk_index < (num_samples / chunk_size); chunk_index++) {
        if (this->read_chunk == this->write_chunk) {
            // CPU was falling behind, add a block of silence
            clear(dst_ptr, chunk_size * sizeof(float));
        }
        else if (mix) {
            // mix new sound data with existing buffer content
            float* src_ptr = &(this->buf[this->read_chunk][0]);
            for (int i = 0; i < chunk_size; i++) {
                dst_ptr[i] += src_ptr[i];
            }
            this->read_chunk = (this->read_chunk + 1) & (num_chunks-1);
        }
        else {
            // copy copy new sound data
            float* src_ptr = &(this->buf[this->read_chunk][0]);
            memcpy(dst_ptr, src_ptr, chunk_size * sizeof(float));
            this->read_chunk = (this->read_chunk + 1) & (num_chunks-1);
        }
        dst_ptr += chunk_size;
    }
}

} // namespace YAKC

