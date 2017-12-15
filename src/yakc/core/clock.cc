//------------------------------------------------------------------------------
//  clock.cc
//------------------------------------------------------------------------------
#include "core.h"
#include "clock.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
clock::init(int khz) {
    YAKC_ASSERT(khz > 0);
    this->base_freq_khz = khz;
}

//------------------------------------------------------------------------------
int64_t
clock::cycles(int micro_seconds) const {
    YAKC_ASSERT(this->base_freq_khz > 0);
    return (int64_t(this->base_freq_khz) * micro_seconds) / 1000;
}

} // namespace YAKC
