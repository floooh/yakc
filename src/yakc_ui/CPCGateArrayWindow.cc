//------------------------------------------------------------------------------
//  CPCGateArrayWindow.cc
//------------------------------------------------------------------------------
#include "CPCGateArrayWindow.h"
#include "yakc_ui/UI.h"
#include "yakc_ui/Util.h"
#include "yakc/emus/cpc.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
CPCGateArrayWindow::Setup(yakc& emu) {
    this->setName("CPC Gate Array Debugger");
}

//------------------------------------------------------------------------------
void
CPCGateArrayWindow::drawColors() {
/*
    const ImVec2 size(12, 12);
    ImGui::Text("Palette Colors:");
    for (int i = 0; i < 16; i++) {
        this->paletteColors[i] = Util::RGBA8toImVec4(cpc.sys.ga.palette[i]);
        ImGui::PushID(i);
        ImGui::ColorButton("##palette_color", this->paletteColors[i], ImGuiColorEditFlags_NoAlpha, size);
        ImGui::PopID();
        if (i != 15) {
            ImGui::SameLine();
        }
    }
    ImGui::Text("Border Color: ");
    this->borderColor = Util::RGBA8toImVec4(cpc.sys.ga.border_color);
    ImGui::ColorButton("##border_color", this->borderColor, ImGuiColorEditFlags_NoAlpha, size);
*/
}

//------------------------------------------------------------------------------
/*
static const char* video_mode_names[4] = {
    "160 x 200 @ 16 colors",
    "320 x 200 @ 4 colors",
    "620 x 200 @ 2 colors",
    "160 x 200 @ 4 colors"
};
*/

bool
CPCGateArrayWindow::Draw(yakc& emu) {
/*
    ImGui::SetNextWindowSize(ImVec2(336, 240), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        ImGui::Checkbox("Debug Visualization", &cpc.sys.video_debug_enabled);
        this->drawColors();
        ImGui::Text("Video Mode: %d (%s)", cpc.sys.ga.video_mode, video_mode_names[cpc.sys.ga.video_mode & 3]);
        ImGui::Text("CRT HSync:  %s  VSync:  %s\n", cpc.sys.crt.h_sync?"ON ":"OFF", cpc.sys.crt.v_sync?"ON ":"OFF");
        ImGui::Text("CRT HBlank: %s  VBlank: %s\n", cpc.sys.crt.h_blank?"ON ":"OFF", cpc.sys.crt.v_blank?"ON ":"OFF");
        ImGui::Text("CRT HPos:   %2d   VPos:   %3d\n", cpc.sys.crt.h_pos, cpc.sys.crt.v_pos);
        ImGui::Text("GA Sync:    %s", cpc.sys.ga.sync?"ON ":"OFF");
        ImGui::Text("GA IRQ Counter: %d", cpc.sys.ga.hsync_irq_counter);
        ImGui::Text("GA INT: %s", cpc.sys.ga.intr?"ON ":"OFF");
    }
    ImGui::End();
*/
    return this->Visible;
}

} // namespace YAKC
