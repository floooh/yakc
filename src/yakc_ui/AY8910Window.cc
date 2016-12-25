//------------------------------------------------------------------------------
//  AY8910Window.cc
//------------------------------------------------------------------------------
#include "AY8910Window.h"
#include "IMUI/IMUI.h"
#include "yakc/sound_ay8910.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
AY8910Window::Setup(yakc& emu) {
    this->setName("AY-3-8910 State");
}

//------------------------------------------------------------------------------
bool
AY8910Window::Draw(yakc& emu) {
    sound_ay8910* snd = nullptr;
    if (emu.cpc.on) {
        snd = &emu.cpc.audio;
    }
    ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        if (snd) {
            ImGui::Text("Period A:     0x%04X", (snd->regs[sound_ay8910::TONE_PERIOD_A_COARSE]<<8)|snd->regs[sound_ay8910::TONE_PERIOD_A_FINE]);
            ImGui::Text("Period B:     0x%04X", (snd->regs[sound_ay8910::TONE_PERIOD_B_COARSE]<<8)|snd->regs[sound_ay8910::TONE_PERIOD_B_FINE]);
            ImGui::Text("Period C:     0x%04X", (snd->regs[sound_ay8910::TONE_PERIOD_C_COARSE]<<8)|snd->regs[sound_ay8910::TONE_PERIOD_C_FINE]);
            ImGui::Text("Noise Period: 0x%02X", snd->regs[sound_ay8910::NOISE_PERIOD]);
            ImGui::Text("Enable:       0x%02X", snd->regs[sound_ay8910::ENABLE]);
            ImGui::Text("Amp A:        0x%02X", snd->regs[sound_ay8910::AMP_A]);
            ImGui::Text("Amp B:        0x%02X", snd->regs[sound_ay8910::AMP_B]);
            ImGui::Text("Amp C:        0x%02X", snd->regs[sound_ay8910::AMP_C]);
            ImGui::Text("Period Env:   0x%04X", (snd->regs[sound_ay8910::ENV_PERIOD_COARSE]<<8)|snd->regs[sound_ay8910::ENV_PERIOD_FINE]);
            ImGui::Text("Env Cycle:    0x%02X", snd->regs[sound_ay8910::ENV_SHAPE_CYCLE]);
            ImGui::Text("IO Port A:    0x%02X", snd->regs[sound_ay8910::IO_PORT_A]);
            ImGui::Text("IO Port B:    0x%02X", snd->regs[sound_ay8910::IO_PORT_B]);
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
