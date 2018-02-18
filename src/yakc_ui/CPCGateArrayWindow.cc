//------------------------------------------------------------------------------
//  CPCGateArrayWindow.cc
//------------------------------------------------------------------------------
#include "CPCGateArrayWindow.h"
#include "yakc_ui/UI.h"
#include "yakc/systems/cpc.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
CPCGateArrayWindow::Setup(yakc& emu) {
    this->setName("CPC Gate Array Debugger");
}

//------------------------------------------------------------------------------
void
CPCGateArrayWindow::drawMainWindow(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(460, 400), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        if (ImGui::Button("Colors...")) {
            this->colorWindowOpen = !this->colorWindowOpen;
        }
        ImGui::SameLine();
        ImGui::Checkbox("Debug Visualization", &cpc.debug_video);
    }
    ImGui::End();
}

//------------------------------------------------------------------------------
static ImVec4
uint32ToImVec4(uint32_t c) {
    ImVec4 v;
    v.x = float(c & 0xFF) / 255.0f;
    v.y = float((c>>8) & 0xFF) / 255.0f;
    v.z = float((c>>16) & 0xFF) / 255.0f;
    v.w = float((c>>16) & 0xFF) / 255.0f;
    return v;
}

//------------------------------------------------------------------------------
void
CPCGateArrayWindow::drawColorWindow(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(440, 180), ImGuiSetCond_Once);
    if (ImGui::Begin("CPC Colors", &this->colorWindowOpen, ImGuiWindowFlags_NoResize)) {
        // draw the hardware colors
        ImGui::Text("Hardware Colors:");
        for (int y = 0, i = 0; y < 2; y++) {
            for (int x = 0; x < 16; x++, i++) {
                this->hwColors[i] = uint32ToImVec4(cpc.ga_colors[i]);
                ImGui::PushID(i);
                ImGui::ColorButton("##hw_color", this->hwColors[i], ImGuiColorEditFlags_NoAlpha);
                ImGui::PopID();
                if (x != 15) {
                    ImGui::SameLine();
                }
            }
        }
        ImGui::Text("Current Palette:");
        for (int i = 0; i < 16; i++) {
            this->paletteColors[i] = uint32ToImVec4(cpc.ga_palette[i]);
            ImGui::PushID(i);
            ImGui::ColorButton("##palette_color", this->hwColors[i], ImGuiColorEditFlags_NoAlpha);
            ImGui::PopID();
            if (i != 15) {
                ImGui::SameLine();
            }
        }
        ImGui::Text("Border Color:");
        this->borderColor = uint32ToImVec4(cpc.ga_border_color);
        ImGui::ColorButton("##border_color", this->borderColor, ImGuiColorEditFlags_NoAlpha);
    }
    ImGui::End();
}

//------------------------------------------------------------------------------
bool
CPCGateArrayWindow::Draw(yakc& emu) {
    this->drawMainWindow(emu);
    if (this->colorWindowOpen) {
        this->drawColorWindow(emu);
    }
    return this->Visible;
}

} // namespace YAKC
