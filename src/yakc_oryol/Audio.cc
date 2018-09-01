//------------------------------------------------------------------------------
//  Audio.cc
//------------------------------------------------------------------------------
#include "Audio.h"
#include "soloud.h"
#include "yakc/util/breadboard.h"

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
        soloud->init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::AUTO, 44100, 512, 1);
        board.audio_sample_rate = soloud->getBackendSamplerate();
    }
    soloud_open_count++;
    this->audioSource = Memory::New<AudioSource>();
    this->audioSource->emu = emu_;
    this->audioSource->setSingleInstance(true);
    this->audioSource->cpu_clock_speed = board.freq_hz;
    this->audioHandle = soloud->play(*this->audioSource, 1.0f);
}

//------------------------------------------------------------------------------
void
Audio::Discard() {
    soloud_open_count--;
    if (soloud_open_count == 0) {
        soloud->stopAll();
        Memory::Delete(this->audioSource);
        this->audioSource = nullptr;
        soloud->deinit();
        Memory::Delete(soloud);
        soloud = nullptr;
    }
}

//------------------------------------------------------------------------------
void
Audio::Update() {
    this->audioSource->cpu_clock_speed = board.freq_hz;
}

//------------------------------------------------------------------------------
uint64_t
Audio::GetProcessedCycles() const {
    return this->audioSource->sample_cycle_count;
}

} // namespace YAKC

