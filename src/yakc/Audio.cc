//------------------------------------------------------------------------------
//  Audio.cc
//------------------------------------------------------------------------------
#include "Audio.h"
#include "soloud.h"

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
Audio::Setup(kc85& kc_) {
    this->kc = &kc_;
    this->kc->audio.setup_callbacks(this, cb_sound, cb_volume, cb_stop);
    this->filter.setParams(SoLoud::BiquadResonantFilter::LOWPASS, 44100, 3000.0f, 2.0f);
    #if ORYOL_EMSCRIPTEN
    this->soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::AUTO, 44100, 256, 2);
    #else
    this->soloud.init(SoLoud::Soloud::CLIP_ROUNDOFF);
    #endif
    this->audioSource.setSingleInstance(true);
    this->audioSource.setFilter(0, &this->filter);
    this->audioSource.sample_rate = this->soloud.getBackendSamplerate();
    this->audioHandle = this->soloud.play(this->audioSource, 1.0f);
    Log::Info("Soloud backend sample rate: %d\n", this->audioSource.sample_rate);
}

//------------------------------------------------------------------------------
void
Audio::Discard() {
    this->soloud.deinit();
    this->kc = nullptr;
}

//------------------------------------------------------------------------------
void
Audio::UpdateCpuCycles(uint32_t cpu_clock_speed, uint32_t cpu_cycles) {
    this->audioSource.set_cpu_cycle_count_and_clock_speed(cpu_clock_speed, cpu_cycles);
}

//------------------------------------------------------------------------------
void
Audio::cb_sound(void* userdata, uint64_t cycle_pos, int channel, int hz) {
    o_assert_range_dbg(channel, 2);
    o_assert_dbg(hz >= 0);
    Audio* self = (Audio*) userdata;
    kc85_audiosource::op op;
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
    kc85_audiosource::op op;
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
