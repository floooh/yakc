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
#include "yakc/core.h"
#include "yakc/z80ctc.h"

namespace YAKC {

class kc85_audio {
public:
    /// callback to start sound or change frequency (channel is 0 or 1)
    typedef void (*sound_cb)(void* userdata, uint64_t cycle_count, int channel, int hz);
    /// callback to stop a sound
    typedef void (*stop_cb)(void* userdata, uint64_t cycle_count, int channel);
    /// callback to set volume (0..31)
    typedef void (*volume_cb)(void* userdata, uint64_t cycle_count, int vol);

    /// setup callbacks (call before anything else)
    void setup_callbacks(void* userdata, sound_cb cb_sound, volume_cb cb_volume, stop_cb cb_stop);
    /// initialize the audio hardware (we need access to the ctc
    void init(z80ctc* ctc);
    /// reset the audio hardware
    void reset();
    /// called from instruction execution loop to get correct T cycle in 60Hzframe
    void update_cycles(uint64_t cycle_count);
    /// update volume (0..1F, called from kc85 PIO-B handler)
    void update_volume(int vol);

    /// update a sound channel when ctc state has changed
    void update_channel(int channel);
    /// must be connected to z80ctc::connect_write0()
    static void ctc_write0(void* userdata);
    /// must be connected to z80ctc::connect_write1()
    static void ctc_write1(void* userdata);

    z80ctc* ctc = nullptr;
    uint64_t cycle_count = 0;       // current absolute CPU cycle count

    int volume = 0;
    sound_cb cb_sound = nullptr;
    volume_cb cb_volume = nullptr;
    stop_cb cb_stop = nullptr;
    void* userdata = nullptr;

    static const int num_channels = 2;
    struct channel_state {
        ubyte ctc_mode = z80ctc::RESET;
        ubyte ctc_constant = 0;
    } channels[num_channels];
};

} // namespace YAKC
