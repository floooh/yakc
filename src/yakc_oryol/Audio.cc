//------------------------------------------------------------------------------
//  Audio.cc
//------------------------------------------------------------------------------
#include "Audio.h"
#include "soloud.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
Audio::Setup(const clock& clk) {
    this->filter.setParams(SoLoud::BiquadResonantFilter::LOWPASS, 44100, 3000.0f, 2.0f);
    this->soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::AUTO, 44100, 1024, 2);
    this->audioSource.setSingleInstance(true);
    this->audioSource.setFilter(0, &this->filter);
    this->audioSource.sample_rate = this->soloud.getBackendSamplerate();
    this->audioSource.cpu_clock_speed = clk.base_freq_khz * 1000;
    this->audioHandle = this->soloud.play(this->audioSource, 1.0f);
}

//------------------------------------------------------------------------------
void
Audio::Discard() {
    this->soloud.deinit();
}

//------------------------------------------------------------------------------
void
Audio::Update(const clock& clk) {
    this->audioSource.cpu_clock_speed = clk.base_freq_khz * 1000;
}

//------------------------------------------------------------------------------
uint64_t
Audio::GetProcessedCycles() const {
    return this->audioSource.sample_cycle_count;
}

//------------------------------------------------------------------------------
void
Audio::cb_sound(void* userdata, uint64_t cycle_pos, int channel, int hz) {
    o_assert_range_dbg(channel, 2);
    o_assert_dbg(hz >= 0);
    Audio* self = (Audio*) userdata;
    AudioSource::op op;
    op.cycle_pos = cycle_pos;
    op.hz = hz;
    op.vol = 0x1f;
    self->audioSource.channels[channel].push(op);
}

//------------------------------------------------------------------------------
void
Audio::cb_stop(void* userdata, uint64_t cycle_pos, int channel) {
    o_assert_range_dbg(channel, 2);
    Audio* self = (Audio*) userdata;
    AudioSource::op op;
    op.cycle_pos = cycle_pos;
    op.hz = 0;
    op.vol = 0;
    self->audioSource.channels[channel].push(op);
}

//------------------------------------------------------------------------------
void
Audio::cb_volume(void* userdata, uint64_t cycle_pos, int vol) {
/*
    Ignore volume changes completely for now, some (of my) games
    seem to set the volume to 0, not sure yet what's up with that.
*/
}

} // namespace YAKC

