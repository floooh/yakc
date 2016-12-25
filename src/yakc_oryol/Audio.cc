//------------------------------------------------------------------------------
//  Audio.cc
//------------------------------------------------------------------------------
#include "Audio.h"
#include "soloud.h"

using namespace Oryol;

namespace YAKC {

SoLoud::Soloud* Audio::soloud = nullptr;
int Audio::soloud_open_count = 0;

//------------------------------------------------------------------------------
void
Audio::Setup(yakc* emu_) {
    this->emu = emu_;
    if (nullptr == soloud) {
        soloud = Memory::New<SoLoud::Soloud>();
        soloud->init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::AUTO, 44100, 1024, 1);
    }
    soloud_open_count++;
    this->filter.setParams(SoLoud::BiquadResonantFilter::LOWPASS, 44100, 3000.0f, 2.0f);
    this->audioSource.emu = emu_;
    this->audioSource.setSingleInstance(true);
    this->audioSource.setFilter(0, &this->filter);
    this->audioSource.cpu_clock_speed = this->emu->board.clck.base_freq_khz * 1000;
    this->audioHandle = soloud->play(this->audioSource, 1.0f);
}

//------------------------------------------------------------------------------
void
Audio::Discard() {
    soloud_open_count--;
    if (soloud_open_count == 0) {
        soloud->deinit();
        Memory::Delete(soloud);
        soloud = nullptr;
    }
}

//------------------------------------------------------------------------------
void
Audio::Update() {
    this->audioSource.cpu_clock_speed = this->emu->board.clck.base_freq_khz * 1000;
}

//------------------------------------------------------------------------------
uint64_t
Audio::GetProcessedCycles() const {
    return this->audioSource.sample_cycle_count;
}

} // namespace YAKC

