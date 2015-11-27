//------------------------------------------------------------------------------
//  Audio.cc
//------------------------------------------------------------------------------
#include "Audio.h"
#include "Synth/Synth.h"

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
Audio::Setup(kc85& kc_) {
    this->kc = &kc_;
    this->kc->ctc.connect_constant_changed0(ctc0_const_changed, this);
    this->kc->ctc.connect_constant_changed1(ctc1_const_changed, this);
}

//------------------------------------------------------------------------------
void
Audio::Discard() {
    this->kc = nullptr;
}

//------------------------------------------------------------------------------
void
Audio::Update() {
    // switch off any channels?
    for (int channel = 0; channel < 2; channel++) {
        const auto& chn = this->kc->ctc.channels[channel];
        if (!(this->ctc_state[channel].mode & z80ctc::RESET) && (chn.mode & z80ctc::RESET)) {
            SynthOp op;
            op.Op = SynthOp::Nop;
            Synth::AddOp(channel, 0, op, 0);
            this->ctc_state[channel].mode = chn.mode;
        }
    }
    Synth::Update();
}

//------------------------------------------------------------------------------
void
Audio::updateVoice(int channel) {
    YAKC_ASSERT(channel < 2);
    const auto& chn = this->kc->ctc.channels[channel];
    if ((chn.constant != this->ctc_state[channel].constant) ||
        ((chn.mode ^ this->ctc_state[channel].mode) && z80ctc::RESET)) {

        if (!(chn.mode & z80ctc::RESET)) {
            const float synthRate = 88.2f; // FIXME: should be 44.1?
            const float clockRate = float(this->kc->clck.baseFreqKHz);
            const int tickDiv = int(clockRate / synthRate);
            unsigned int synthTickOffset = this->kc->cycles_frame / tickDiv;
            int div = chn.constant * ((chn.mode & z80ctc::PRESCALER_256) ? 256 : 16);
            float freq = (float(1750000) / float(div)) / 2.0f;
            SynthOp op;
            op.Op = SynthOp::Replace;
            op.Wave = SynthOp::Square;
            op.Freq = freq;
            Synth::AddOp(channel, 0, op, synthTickOffset);
            this->ctc_state[channel].constant = chn.constant;
            this->ctc_state[channel].mode = chn.mode;
        }
    }
}

//------------------------------------------------------------------------------
void
Audio::ctc0_const_changed(void* userdata) {
    Audio* self = (Audio*) userdata;
    self->updateVoice(0);
}

//------------------------------------------------------------------------------
void
Audio::ctc1_const_changed(void* userdata) {
    Audio* self = (Audio*) userdata;
    self->updateVoice(1);
}
