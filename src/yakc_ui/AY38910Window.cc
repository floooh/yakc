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
    ImGui::SetNextWindowSize(ImVec2(220, 270), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        const ay38910_t& ay = board.ay38910;
        const char* type = "unknown";
        switch (ay.type) {
            case AY38910_TYPE_8910: type="AY-3-8910"; break;
            case AY38910_TYPE_8912: type="AY-3-8912"; break;
            case AY38910_TYPE_8913: type="AY-3-8913"; break;
        }
        const uint16_t period_a = (ay.period_a_coarse<<8)|ay.period_a_fine;
        const uint16_t period_b = (ay.period_b_coarse<<8)|ay.period_b_fine;
        const uint16_t period_c = (ay.period_c_coarse<<8)|ay.period_c_fine;
        const uint16_t period_env = (ay.period_env_coarse<<8)|ay.period_env_fine;
        ImGui::Text("Type:         %s", type);
        ImGui::Separator();
        ImGui::Text("              A    B    C");
        ImGui::Text("Tone Periods: %04X %04X %04X", period_a, period_b, period_c);
        ImGui::Text("Tone Amps:    %02X   %02X   %02X", ay.amp_a, ay.amp_b, ay.amp_c);
        ImGui::Text("Noise Period: %02X", ay.period_noise);
        ImGui::Text("Env Period:   %04X", period_env);
        ImGui::Text("Env Cycle:    %02X", ay.env_shape_cycle);
        ImGui::Text("Enable:       %02X", ay.enable);
        ImGui::Text("IO Ports:     %02X   %02X", ay.port_a, ay.port_b);
        ImGui::Separator();
        ImGui::Text("Tone Counter: %04X %04X %04X", ay.tone[0].counter, ay.tone[1].counter, ay.tone[2].counter);
        ImGui::Text("Tone Bit:     %s %s %s",
            ay.tone[0].bit ? "ON  ":"OFF ",
            ay.tone[1].bit ? "ON  ":"OFF ",
            ay.tone[2].bit ? "ON  ":"OFF ");
        ImGui::Text("Tone Enable:  %s %s %s",
            !ay.tone[0].tone_disable ? "ON  ":"OFF ",
            !ay.tone[1].tone_disable ? "ON  ":"OFF ",
            !ay.tone[2].tone_disable ? "ON  ":"OFF ");
        ImGui::Text("Noise Enable: %s %s %s",
            !ay.tone[0].noise_disable ? "ON  ":"OFF ",
            !ay.tone[1].noise_disable ? "ON  ":"OFF ",
            !ay.tone[2].noise_disable ? "ON  ":"OFF ");
        ImGui::Text("Env Counter:  %04X", ay.env.counter);
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
