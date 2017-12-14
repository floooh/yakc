//------------------------------------------------------------------------------
//  MC6845Window.cc
//------------------------------------------------------------------------------
#include "MC6845Window.h"
#include "IMUI/IMUI.h"
#include "yakc/chips/mc6845.h"

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
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
    /*
        const mc6845& mc = emu.board.mc6845;
        ImGui::Text("H Total:       0x%02X", mc.regs[mc6845::H_TOTAL]);
        ImGui::Text("H Displayed:   0x%02X", mc.regs[mc6845::H_DISPLAYED]);
        ImGui::Text("H Sync Pos:    0x%02X", mc.regs[mc6845::H_SYNC_POS]);
        ImGui::Text("H Sync Width:  0x%02X", mc.regs[mc6845::SYNC_WIDTHS]);
        ImGui::Text("V Total:       0x%02X", mc.regs[mc6845::V_TOTAL]);
        ImGui::Text("V Total Adj:   0x%02X", mc.regs[mc6845::V_TOTAL_ADJUST]);
        ImGui::Text("V Displayed:   0x%02X", mc.regs[mc6845::V_DISPLAYED]);
        ImGui::Text("V Sync Pos:    0x%02X", mc.regs[mc6845::V_SYNC_POS]);
        ImGui::Text("Interl Mode:   0x%02X", mc.regs[mc6845::INTERLACE_MODE]);
        ImGui::Text("Max Scanline:  0x%02X", mc.regs[mc6845::MAX_SCANLINE_ADDR]);
        ImGui::Text("Cursor Start:  0x%02X", mc.regs[mc6845::CURSOR_START]);
        ImGui::Text("Cursor End:    0x%02X", mc.regs[mc6845::CURSOR_END]);
        ImGui::Text("Start Addr:    0x%04X", (mc.regs[mc6845::START_ADDR_HI]<<8)|mc.regs[mc6845::START_ADDR_LO]);
        ImGui::Text("Cursor Addr:   0x%04X", (mc.regs[mc6845::CURSOR_HI]<<8)|mc.regs[mc6845::CURSOR_LO]);
        ImGui::Text("Lightpen Addr: 0x%04X", (mc.regs[mc6845::LIGHTPEN_HI]<<8)|mc.regs[mc6845::LIGHTPEN_LO]);
    */
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC


