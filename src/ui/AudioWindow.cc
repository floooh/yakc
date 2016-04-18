//------------------------------------------------------------------------------
//  AudioWindow.cc
//------------------------------------------------------------------------------
#include "AudioWindow.h"
#include "IMUI/IMUI.h"
#include "soloud.h"
#include "yakc/Audio.h"

OryolClassImpl(AudioWindow);

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
AudioWindow::AudioWindow(Audio* audio_) :
audio(audio_),
paused(false),
sample_cycle_count(0),
cpu_cycle_count(0),
diffIndex(0) {
    Memory::Clear(&this->wavBuffer, sizeof(this->wavBuffer));
    Memory::Clear(&this->diffBuffer, sizeof(this->diffBuffer));
}

//------------------------------------------------------------------------------
void
AudioWindow::Setup(kc85& kc) {
    this->setName("Audio Debugger");
}

//------------------------------------------------------------------------------
bool
AudioWindow::Draw(kc85& kc) {
    ImGui::SetNextWindowSize(ImVec2(600, 300), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        ImGui::Checkbox("Pause", &this->paused);
        if (!this->paused) {
            Memory::Copy(this->audio->soloud.getWave(), this->wavBuffer, 256*sizeof(float));
            this->sample_cycle_count = this->audio->audioSource.cur_sample_cycle_count;
            this->cpu_cycle_count = this->audio->audioSource.cur_cpu_cycle_count;
            this->diffBuffer[this->diffIndex] = float(this->cpu_cycle_count-this->sample_cycle_count);
            this->diffIndex = (this->diffIndex + 1) & 255;
        }
        ImGui::Text("CPU/Audio Diff Cycles: %d\n", int32_t(this->cpu_cycle_count - this->sample_cycle_count));
        ImGui::PlotLines("Wave", this->wavBuffer, 256, 0, nullptr, -1, 1, ImVec2(512, 60));
        ImGui::PlotLines("Diff", this->diffBuffer, 256, 0, nullptr, -100000, 400000.0, ImVec2(512, 120));
    }
    ImGui::End();
    return this->Visible;
}
