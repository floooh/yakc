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
CPCGateArrayWindow::drawColors() {
    const ImVec2 size(12, 12);
    ImGui::Text("Palette Colors:");
    for (int i = 0; i < 16; i++) {
        this->paletteColors[i] = uint32ToImVec4(cpc.ga_palette[i]);
        ImGui::PushID(i);
        ImGui::ColorButton("##palette_color", this->paletteColors[i], ImGuiColorEditFlags_NoAlpha, size);
        ImGui::PopID();
        if (i != 15) {
            ImGui::SameLine();
        }
    }
    ImGui::Text("Border Color: ");
    this->borderColor = uint32ToImVec4(cpc.ga_border_color);
    ImGui::ColorButton("##border_color", this->borderColor, ImGuiColorEditFlags_NoAlpha, size);
}

//------------------------------------------------------------------------------
static const char* video_mode_names[4] = {
    "160 x 200 @ 16 colors",
    "320 x 200 @ 4 colors",
    "620 x 200 @ 2 colors",
    "160 x 200 @ 4 colors"
};

bool
CPCGateArrayWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(336, 240), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        ImGui::Checkbox("Debug Visualization", &cpc.debug_video);
        this->drawColors();
        ImGui::Text("Video Mode: %d (%s)", cpc.ga_video_mode, video_mode_names[cpc.ga_video_mode & 3]);
        ImGui::Text("CRT HSync:  %s  VSync:  %s\n", board.crt.h_sync?"ON ":"OFF", board.crt.v_sync?"ON ":"OFF");
        ImGui::Text("CRT HBlank: %s  VBlank: %s\n", board.crt.h_blank?"ON ":"OFF", board.crt.v_blank?"ON ":"OFF");
        ImGui::Text("CRT HPos:   %2d   VPos:   %3d\n", board.crt.h_pos, board.crt.v_pos);
        ImGui::Text("GA Sync:    %s", cpc.ga_sync?"ON ":"OFF");
        ImGui::Text("GA IRQ Counter: %d", cpc.ga_hsync_irq_counter);
        ImGui::Text("GA INT: %s", cpc.ga_int?"ON ":"OFF");
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
