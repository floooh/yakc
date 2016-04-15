//------------------------------------------------------------------------------
//  Audio.cc
//------------------------------------------------------------------------------
#include "Audio.h"

using namespace Oryol;
using namespace yakc;
using namespace SoLoud;

//------------------------------------------------------------------------------
Audio::kcAudioInstance::kcAudioInstance(kcAudio* aParent) :
parent(aParent) {
    this->freqCounter[0] = 0;
    this->freqCounter[1] = 0;
}

//------------------------------------------------------------------------------
void
Audio::kcAudioInstance::getAudio(float* aBuffer, unsigned int aSamples) {
    // NOTE: this may be called from a thread
    if (this->parent->freq[0] > 0) {
        uint16_t cnt = this->freqCounter[0];
        uint16_t add = 0x10000 / (44100 / this->parent->freq[0]);
        for (uint32_t i = 0; i < aSamples; i++) {
            cnt = (cnt + add) & 0xFFFF;
            *aBuffer++ = cnt < 0x8000 ? 0.5f : -0.5f;
        }
        this->freqCounter[0] = cnt;
    }
    else {
        for (uint32_t i = 0; i < aSamples; i++) {
            *aBuffer++ = 0;
        }
    }
    if (this->parent->freq[1] > 0) {
        uint16_t cnt = this->freqCounter[1];
        uint16_t add = 0x10000 / (44100 / this->parent->freq[1]);
        for (uint32_t i = 0; i < aSamples; i++) {
            cnt = (cnt + add) & 0xFFFF;
            *aBuffer++ = cnt < 0x8000 ? 0.5f : -0.5f;
        }
        this->freqCounter[1] = cnt;
    }
    else {
        for (uint32_t i = 0; i < aSamples; i++) {
            *aBuffer++ = 0;
        }
    }
}

//------------------------------------------------------------------------------
bool
Audio::kcAudioInstance::hasEnded() {
    return false;
}

//------------------------------------------------------------------------------
void
Audio::Setup(kc85& kc_) {
    this->kc = &kc_;
    this->kc->audio.setup_callbacks(this, cb_sound, cb_volume, cb_stop);
    this->soloud.init(Soloud::CLIP_ROUNDOFF);
    this->audioSource.setSingleInstance(true);
    this->audioHandle = this->soloud.play(this->audioSource, 1.0f);
}

//------------------------------------------------------------------------------
void
Audio::Discard() {
    this->soloud.deinit();
    this->kc = nullptr;
}

//------------------------------------------------------------------------------
void
Audio::Update(int micro_secs) {
//    this->audioSource.frameSamplePos += ((44.1 * micro_secs) / 1000.0) + 0.5;
}

//------------------------------------------------------------------------------
/*
unsigned int
Audio::asNumAudioSamples(unsigned int z80Cycles) const {
    const double sampleRate = 44.1;
    const double clockRate = this->kc->clck.base_freq_khz;


    const unsigned int tickDiv = (unsigned int)(clockRate / synthRate);
    return (z80Cycles / tickDiv) + BaseDelayTicks;
}
*/

//------------------------------------------------------------------------------
void
Audio::cb_sound(void* userdata, unsigned int delay_cycles, int channel, int hz) {
    o_assert_range_dbg(channel, 2);
    Audio* self = (Audio*) userdata;
    self->audioSource.freq[channel] = hz;
/*
    Audio* self = (Audio*) userdata;
    const unsigned int audioDelayTicks = self->asAudioTicks(delay_cycles);
    SynthOp op;
    op.Op = SynthOp::Replace;
    op.Wave = SynthOp::Square;
    op.Freq = hz;
    Synth::AddOp(channel, 0, op, audioDelayTicks);
*/
}

//------------------------------------------------------------------------------
void
Audio::cb_stop(void* userdata, unsigned int delay_cycles, int channel) {
    o_assert_range_dbg(channel, 2);
    Audio* self = (Audio*) userdata;
    self->audioSource.freq[channel] = 0;
/*
    Audio* self = (Audio*) userdata;
    const unsigned int audioDelayTicks = self->asAudioTicks(delay_cycles);
    SynthOp op;
    op.Op = SynthOp::Nop;
    Synth::AddOp(channel, 0, op, audioDelayTicks);
*/
}

//------------------------------------------------------------------------------
void
Audio::cb_volume(void* userdata, unsigned int delay_cycles, int vol) {
/*
    Ignore volume changes completely for now, some (of my) games
    seem to set the volume to 0, not sure yet what's up with that.
*/
}
