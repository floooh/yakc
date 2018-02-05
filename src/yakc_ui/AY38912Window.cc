//------------------------------------------------------------------------------
//  AY38912Window.cc
//------------------------------------------------------------------------------
#include "AY38912Window.h"
#include "IMUI/IMUI.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
AY38912Window::Setup(yakc& emu) {
    this->setName("AY-3-8912 State");
}

//------------------------------------------------------------------------------
bool
AY38912Window::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(200, 240), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        const ay38912_t& ay = board.ay38912;
        ImGui::Text("Period A:     0x%04X", (ay.reg[AY38912_REG_PERIOD_A_COARSE]<<8)|ay.reg[AY38912_REG_PERIOD_A_FINE]);
        ImGui::Text("Period B:     0x%04X", (ay.reg[AY38912_REG_PERIOD_B_COARSE]<<8)|ay.reg[AY38912_REG_PERIOD_B_FINE]);
        ImGui::Text("Period C:     0x%04X", (ay.reg[AY38912_REG_PERIOD_C_COARSE]<<8)|ay.reg[AY38912_REG_PERIOD_C_FINE]);
        ImGui::Text("Noise Period: 0x%02X", ay.reg[AY38912_REG_PERIOD_NOISE]);
        ImGui::Text("Enable:       0x%02X", ay.reg[AY38912_REG_ENABLE]);
        ImGui::Text("Amp A:        0x%02X", ay.reg[AY38912_REG_AMP_A]);
        ImGui::Text("Amp B:        0x%02X", ay.reg[AY38912_REG_AMP_B]);
        ImGui::Text("Amp C:        0x%02X", ay.reg[AY38912_REG_AMP_C]);
        ImGui::Text("Period Env:   0x%04X", (ay.reg[AY38912_REG_ENV_PERIOD_COARSE]<<8)|ay.reg[AY38912_REG_ENV_PERIOD_FINE]);
        ImGui::Text("Env Cycle:    0x%02X", ay.reg[AY38912_REG_ENV_SHAPE_CYCLE]);
        ImGui::Text("IO Port A:    0x%02X", ay.reg[AY38912_REG_IO_PORT_A]);
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
