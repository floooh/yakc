//------------------------------------------------------------------------------
//  AudioWindow.cc
//------------------------------------------------------------------------------
#include "AudioWindow.h"
#include "IMUI/IMUI.h"
#include "soloud.h"
#include "yakc_oryol/Audio.h"
#include "yakc_ui/UI.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
AudioWindow::AudioWindow(Audio* audio_) :
audio(audio_),
paused(false),
cpuAhead(false),
cpuBehind(false) {
    Memory::Clear(&this->wavBuffer, sizeof(this->wavBuffer));
}

//------------------------------------------------------------------------------
void
AudioWindow::Setup(yakc& emu) {
    this->setName("Audio Debugger");
}

//------------------------------------------------------------------------------
bool
AudioWindow::Draw(yakc& emu) {
    o_assert_dbg(Audio::soloud);
    ImGui::SetNextWindowSize(ImVec2(600, 220), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        Audio::soloud->setVisualizationEnable(true);
        ImGui::Checkbox("Pause", &this->paused);
        if (!this->paused) {
            Memory::Copy(Audio::soloud->getWave(), this->wavBuffer, 256*sizeof(float));
            this->cpuAhead = emu.kc85.cpu_ahead;
            this->cpuBehind = emu.kc85.cpu_behind;
        }
        ImGui::Text("Backend: %s", Audio::soloud->getBackendString());
        ImGui::Text("Backend sample rate: source=%d, actual=%d\n",  this->audio->audioSource.sample_rate, 
            Audio::soloud->getBackendSamplerate());
        ImGui::Text("Backend sample buffer size: %d\n", Audio::soloud->getBackendBufferSize());
        if (this->cpuAhead) {
            ImGui::TextColored(UI::WarnColor, "*** CPU AHEAD ***");
        }
        else if (this->cpuBehind) {
            ImGui::TextColored(UI::WarnColor, "*** CPU BEHIND ***");
        }
        else {
            ImGui::TextColored(UI::OkColor, "CPU SYNCED");
        }
        for (int chn=0; chn<2; chn++) {
            if (this->audio->audioSource.channels[chn].overflow) {
                ImGui::TextColored(UI::WarnColor, "*** CHANNEL %d: RINGBUFFER OVERFLOW***", chn);
            }
            else {
                ImGui::TextColored(UI::OkColor, "Channel %d: ok", chn);
            }
        }
        ImGui::PlotLines("Wave", this->wavBuffer, 256, 0, nullptr, -1, 1, ImVec2(512, 60));
    }
    else {
        Audio::soloud->setVisualizationEnable(false);
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
