#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::keybuffer
    @brief keyboard buffer helper class

    A simple FIFO buffer for key strokes which makes sure that keys
    are returned as pressed for a minimal amount of time.
*/
#include "yakc/core.h"

namespace YAKC {

class keybuffer {
public:
    /// constructor
    keybuffer();
    /// initialize the keybuffer, delay
    void init(int delay);
    /// reset the key buffer
    void reset();
    /// advance the internal timer (call once per frame)
    void advance();
    /// push a key code
    void write(uint8_t key);
    /// read key code (same as last time, or a new one)
    uint8_t read();

    static const int num_entries = 32;  // must be 2^N
    static const int wrap_mask = num_entries-1;
    int advance_delay;
    int advance_count;                  // counter until read position advances
    int write_pos;                      // current write position
    int read_pos;                       // current read position
    uint8_t buf[num_entries];
};

} // namespace YAKC
