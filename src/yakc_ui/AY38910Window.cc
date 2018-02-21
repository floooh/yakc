//------------------------------------------------------------------------------
//  AY38910Window.cc
//------------------------------------------------------------------------------
#include "AY38910Window.h"
#include "IMUI/IMUI.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
AY38910Window::Setup(yakc& emu) {
    this->setName("AY-3-8910 State");
}

//------------------------------------------------------------------------------
bool
AY38910Window::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(200, 240), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        const ay38910_t& ay = board.ay38910;
        ImGui::Text("Period A:     0x%04X", (ay.period_a_coarse<<8)|ay.period_a_fine);
        ImGui::Text("Period B:     0x%04X", (ay.period_b_coarse<<8)|ay.period_b_fine);
        ImGui::Text("Period C:     0x%04X", (ay.period_c_coarse<<8)|ay.period_c_fine);
        ImGui::Text("Noise Period: 0x%02X", ay.period_noise);
        ImGui::Text("Enable:       0x%02X", ay.enable);
        ImGui::Text("Amp A:        0x%02X", ay.amp_a);
        ImGui::Text("Amp B:        0x%02X", ay.amp_b);
        ImGui::Text("Amp C:        0x%02X", ay.amp_c);
        ImGui::Text("Period Env:   0x%04X", (ay.period_env_coarse<<8)|ay.period_env_fine);
        ImGui::Text("Env Cycle:    0x%02X", ay.env_shape_cycle);
        ImGui::Text("IO Port A:    0x%02X", ay.port_a);
        ImGui::Text("IO Port B:    0x%02X", ay.port_b);
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
