//------------------------------------------------------------------------------
//  MC6845Window.cc
//------------------------------------------------------------------------------
#include "MC6845Window.h"
#include "IMUI/IMUI.h"
#include "chips/mc6845.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
MC6845Window::Setup(yakc& emu) {
    this->setName("MC6845 State");
}

//------------------------------------------------------------------------------
bool
MC6845Window::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(200, 292), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        const mc6845_t& mc = board.mc6845;
        ImGui::Text("H Total:       0x%02X", mc.h_total);
        ImGui::Text("H Displayed:   0x%02X", mc.h_displayed);
        ImGui::Text("H Sync Pos:    0x%02X", mc.h_sync_pos);
        ImGui::Text("H Sync Width:  0x%02X", mc.sync_widths);
        ImGui::Text("V Total:       0x%02X", mc.v_total);
        ImGui::Text("V Total Adj:   0x%02X", mc.v_total_adjust);
        ImGui::Text("V Displayed:   0x%02X", mc.v_displayed);
        ImGui::Text("V Sync Pos:    0x%02X", mc.v_sync_pos);
        ImGui::Text("Interl Mode:   0x%02X", mc.interlace_mode);
        ImGui::Text("Max Scanline:  0x%02X", mc.max_scanline_addr);
        ImGui::Text("Cursor Start:  0x%02X", mc.cursor_start);
        ImGui::Text("Cursor End:    0x%02X", mc.cursor_end);
        ImGui::Text("Start Addr:    0x%04X", (mc.start_addr_hi<<8)|mc.start_addr_lo);
        ImGui::Text("Cursor Addr:   0x%04X", (mc.cursor_hi<<8)|mc.cursor_lo);
        ImGui::Text("Lightpen Addr: 0x%04X", (mc.lightpen_hi<<8)|mc.lightpen_lo);
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC


