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
#include "core/common.h"

namespace yakc {

class kc85_audio {
public:
    /// callback to start sound or change frequency (channel is 0 or 1)
    typedef void (*sound_cb)(void* userdata, unsigned int delay_cycles, int channel, int hz);
    /// callback to stop a sound
    typedef void (*stop_cb)(void* userdata, unsigned int delay_cycles, int channel);
    /// callback to set volume (0..31)
    typedef void (*volume_cb)(void* userdata, unsigned int delay_cycles, int vol);

    /// setup callbacks (call before anything else)
    void setup_callbacks(void* userdata, sound_cb cb_sound, volume_cb cb_volume, stop_cb cb_stop);
    /// initialize the audio hardware (we need access to the ctc
    void init(z80ctc* ctc);
    /// reset the audio hardware
    void reset();
    /// called from instruction execution loop to get correct T cycle in 60Hzframe
    void update_t(unsigned int t);
    /// update volume (0..1F, called from kc85 PIO-B handler)
    void update_volume(int vol);

    /// update a sound channel when ctc state has changed
    void update_channel(int channel);
    /// must be connected to z80ctc::connect_write0()
    static void ctc_write0(void* userdata);
    /// must be connected to z80ctc::connect_write1()
    static void ctc_write1(void* userdata);

    z80ctc* ctc = nullptr;

    unsigned int frame_t = 0;           // current T cycle inside 60Hz frame

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

//------------------------------------------------------------------------------
inline void
kc85_audio::setup_callbacks(void* ud, sound_cb cb_snd, volume_cb cb_vol, stop_cb cb_stp) {
    this->userdata = ud;
    this->cb_sound = cb_snd;
    this->cb_volume = cb_vol;
    this->cb_stop = cb_stp;
}

//------------------------------------------------------------------------------
inline void
kc85_audio::init(z80ctc* ctc_) {
    YAKC_ASSERT(ctc_);
    this->ctc = ctc_;
    this->ctc->connect_write0(ctc_write0, this);
    this->ctc->connect_write1(ctc_write1, this);
    this->reset();
}

//------------------------------------------------------------------------------
inline void
kc85_audio::reset() {
    if (this->cb_stop) {
        this->cb_stop(userdata, 0, 0);
        this->cb_stop(userdata, 0, 1);
    }
    this->channels[0] = channel_state();
    this->channels[1] = channel_state();
}

//------------------------------------------------------------------------------
inline void
kc85_audio::update_t(unsigned int t) {
    this->frame_t = t;
}

//------------------------------------------------------------------------------
inline void
kc85_audio::update_volume(int vol) {
    if (this->volume != vol) {
        if (this->cb_volume) {
            this->cb_volume(this->userdata, this->frame_t, vol);
        }
    }
    this->volume = vol;
}

//------------------------------------------------------------------------------
inline void
kc85_audio::update_channel(int channel) {

    // has the CTC channel state changed since last time?
    const auto& ctc_chn = this->ctc->channels[channel];
    if ((ctc_chn.constant != this->channels[channel].ctc_constant) ||
        (ctc_chn.mode ^ this->channels[channel].ctc_mode)) {

        if (!(this->channels[channel].ctc_mode & z80ctc::RESET) && (ctc_chn.mode & z80ctc::RESET)) {
            // CTC channel has become inactive, call the stop-callback
            if (this->cb_stop) {
                this->cb_stop(this->userdata, this->frame_t, channel);
            }
            this->channels[channel].ctc_mode = ctc_chn.mode;
        }
        else if (!(ctc_chn.mode & z80ctc::RESET)) {
            // CTC channel has become active or constant has changed, call sound-callback
            int div = ctc_chn.constant * ((ctc_chn.mode & z80ctc::PRESCALER_256) ? 256 : 16);
            float hz = (float(1750000) / float(div)) / 2.0f;
            if (this->cb_sound) {
                this->cb_sound(this->userdata, this->frame_t, channel, (int)hz);
            }
            this->channels[channel].ctc_constant = ctc_chn.constant;
            this->channels[channel].ctc_mode = ctc_chn.mode;
        }
    }
}

//------------------------------------------------------------------------------
inline void
kc85_audio::ctc_write0(void* userdata) {
    kc85_audio* self = (kc85_audio*)userdata;
    self->update_channel(0);
}

//------------------------------------------------------------------------------
inline void
kc85_audio::ctc_write1(void* userdata) {
    kc85_audio* self = (kc85_audio*)userdata;
    self->update_channel(1);
}

} // namespace yakc
