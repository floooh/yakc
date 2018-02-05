//------------------------------------------------------------------------------
//  MOS6522Window.cc
//------------------------------------------------------------------------------
#include "MOS6522Window.h"
#include "IMUI/IMUI.h"
#include "yakc/chips/mos6522.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
MOS6522Window::Setup(yakc& emu) {
    this->setName("MOS6522 State");
}

//------------------------------------------------------------------------------
bool
MOS6522Window::Draw(yakc& emu) {
    const mos6522& via = emu.board.mos6522;
    ImGui::SetNextWindowSize(ImVec2(200, 292), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        ImGui::Text("A OUT:     0x%02X", via.out_a);
        ImGui::Text("A IN:      0x%02X", via.in_a);
        ImGui::Text("A DDR:     0x%02X", via.ddr_a);
        ImGui::Text("B OUT:     0x%02X", via.out_b);
        ImGui::Text("B IN:      0x%02X", via.in_b);
        ImGui::Text("B DDR:     0x%02X", via.ddr_b);
        ImGui::Text("ACR:       0x%02X", via.acr);
        ImGui::Text("  T1 Ctrl:   %d%d", (via.acr>>7)&1, (via.acr>>6)&1);
        ImGui::Text("  T2 Ctrl:    %d", (via.acr>>5)&1);
        ImGui::Text("  SR Ctrl:  %d%d%d", (via.acr>>4)&1,(via.acr>>3)&1,(via.acr>>2)&1);
        ImGui::Text("  B Latch:    %d", (via.acr>>1)&1);
        ImGui::Text("  A Latch:    %d", (via.acr&1));
        ImGui::Text("T1LL:      0x%02X", via.t1ll);
        ImGui::Text("T1LH:      0x%02X", via.t1lh);
        ImGui::Text("T2LL:      0x%02X", via.t2ll);
        ImGui::Text("T2LH:      0x%02X", via.t2lh);
        ImGui::Text("T1:      0x%04X", via.t1);
        ImGui::Text("T2:      0x%04X", via.t2);
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
