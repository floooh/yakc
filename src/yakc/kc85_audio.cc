//------------------------------------------------------------------------------
//  kc85_audio.cc
//------------------------------------------------------------------------------
#include "kc85_audio.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
kc85_audio::setup_callbacks(void* ud, sound_cb cb_snd, volume_cb cb_vol, stop_cb cb_stp) {
    this->userdata = ud;
    this->cb_sound = cb_snd;
    this->cb_volume = cb_vol;
    this->cb_stop = cb_stp;
}

//------------------------------------------------------------------------------
void
kc85_audio::init(z80ctc* ctc_) {
    YAKC_ASSERT(ctc_);
    this->ctc = ctc_;
    this->ctc->connect_write0(ctc_write0, this);
    this->ctc->connect_write1(ctc_write1, this);
    this->reset();
}

//------------------------------------------------------------------------------
void
kc85_audio::reset() {
    if (this->cb_stop) {
        this->cb_stop(userdata, 0, 0);
        this->cb_stop(userdata, 0, 1);
    }
    this->channels[0] = channel_state();
    this->channels[1] = channel_state();
}

//------------------------------------------------------------------------------
void
kc85_audio::update_cycles(uint64_t cycles) {
    this->cycle_count = cycles;
}

//------------------------------------------------------------------------------
void
kc85_audio::update_volume(int vol) {
    if (this->volume != vol) {
        if (this->cb_volume) {
            this->cb_volume(this->userdata, this->cycle_count, vol);
        }
    }
    this->volume = vol;
}

//------------------------------------------------------------------------------
void
kc85_audio::update_channel(int channel) {

    // has the CTC channel state changed since last time?
    const auto& ctc_chn = this->ctc->channels[channel];
    if ((ctc_chn.constant != this->channels[channel].ctc_constant) ||
        (ctc_chn.mode ^ this->channels[channel].ctc_mode)) {

        if (!(this->channels[channel].ctc_mode & z80ctc::RESET) && (ctc_chn.mode & z80ctc::RESET)) {
            // CTC channel has become inactive, call the stop-callback
            if (this->cb_stop) {
                this->cb_stop(this->userdata, this->cycle_count, channel);
            }
            this->channels[channel].ctc_mode = ctc_chn.mode;
        }
        else if (!(ctc_chn.mode & z80ctc::RESET)) {
            // CTC channel has become active or constant has changed, call sound-callback
            int div = ctc_chn.constant * ((ctc_chn.mode & z80ctc::PRESCALER_256) ? 256 : 16);
            if (div > 0) {
                int hz = int((float(1750000) / float(div)) / 2.0f);
                if (this->cb_sound) {
                    this->cb_sound(this->userdata, this->cycle_count, channel, hz);
                }
            }
            this->channels[channel].ctc_constant = ctc_chn.constant;
            this->channels[channel].ctc_mode = ctc_chn.mode;
        }
    }
}

//------------------------------------------------------------------------------
void
kc85_audio::ctc_write0(void* userdata) {
    kc85_audio* self = (kc85_audio*)userdata;
    self->update_channel(0);
}

//------------------------------------------------------------------------------
void
kc85_audio::ctc_write1(void* userdata) {
    kc85_audio* self = (kc85_audio*)userdata;
    self->update_channel(1);
}

} // namespace YAKC
