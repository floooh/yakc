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
    this->kc->audio.setup_callbacks(this, cb_sound, cb_volume, cb_stop);
}

//------------------------------------------------------------------------------
void
Audio::Discard() {
    this->kc = nullptr;
}

//------------------------------------------------------------------------------
unsigned int
Audio::asAudioTicks(unsigned int z80Cycles) const {
    const float synthRate = Synth::SynthSetup().SampleRate / 1000.0f;
    const float clockRate = float(this->kc->clck.base_freq_khz);
    const unsigned int tickDiv = (unsigned int)(clockRate / synthRate);
    return (z80Cycles / tickDiv) + baseDelayTicks;
}

//------------------------------------------------------------------------------
void
Audio::cb_sound(void* userdata, unsigned int delay_cycles, int channel, int hz) {
    Audio* self = (Audio*) userdata;
    const unsigned int audioDelayTicks = self->asAudioTicks(delay_cycles);

    SynthOp op;
    op.Op = SynthOp::Replace;
    op.Wave = SynthOp::Square;
    op.Freq = hz;
    Synth::AddOp(channel, 0, op, audioDelayTicks);
}

//------------------------------------------------------------------------------
void
Audio::cb_stop(void* userdata, unsigned int delay_cycles, int channel) {
    Audio* self = (Audio*) userdata;
    const unsigned int audioDelayTicks = self->asAudioTicks(delay_cycles);

    SynthOp op;
    op.Op = SynthOp::Nop;
    Synth::AddOp(channel, 0, op, audioDelayTicks);
}

//------------------------------------------------------------------------------
void
Audio::cb_volume(void* userdata, unsigned int delay_cycles, int vol) {
/*
    Ignore volume changes completely for now, some (of my) games
    seem to set the volume to 0, not sure yet what's up with that.

    Audio* self = (Audio*) userdata;
    const unsigned int audioDelayTicks = self->asAudioTicks(delay_cycles);

    // vol is 0..31, we need 0..32767
    // at least on KC85/3, volume is only for channel 0(?)
    SynthOp op;
    op.Op   = SynthOp::Modulate;
    op.Wave = SynthOp::Const;
    op.Amp  = (((1<<15) - 1) * vol) / 31;
    Synth::AddOp(0, 1, op, audioDelayTicks);
*/
}
