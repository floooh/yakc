#pragma once
//------------------------------------------------------------------------------
/**
    @class kc85_audio
    @brief provides a callback interface to hook into KC85 audio generation

    The KC85 sound generation is very simple:
    - 2 channels
    - 2 simple oscillators driven by CTC channel 0 and 1
    - 32 volume levels
    
    The emulator wrapper application must provide 3 callbacks:
    - sound_cb: start sound or change frequency on one channel, with
      a frequency in Hz and a delay in 'sample ticks' @44.1 kHz
    - volume_cb: change the volume of one channel
    - stop_cb: stop a sound with delay in sample ticks
*/
#include "yakc/core/core.h"
#include "yakc/systems/breadboard.h"

namespace YAKC {

class kc85_audio {
public:
    /// initialize the audio hardware
    void init(breadboard* board);
    /// reset the audio hardware
    void reset();
    /// called after executing CPU instruction
    void step(int cpu_cycles);
    /// must be called from system_bus::ctc_write() for CTC channel 0 or 1
    void ctc_write(int ctc_channel);

    breadboard* board = nullptr;
    uint64_t cycle_count = 0;       // current absolute CPU cycle count
    static const int num_channels = 2;
    struct channel_state {
        ubyte ctc_mode = z80ctc::RESET;
        ubyte ctc_constant = 0;
    } channels[num_channels];
};

} // namespace YAKC
