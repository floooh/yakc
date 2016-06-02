//------------------------------------------------------------------------------
//  DisasmWindow.cc
//------------------------------------------------------------------------------
#include "DisasmWindow.h"
#include "IMUI/IMUI.h"
#include "yakc_oryol/Disasm.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
DisasmWindow::Setup(yakc& emu) {
    this->setName("Disassembler");
    this->startWidget.Configure16("Start", 0x0000);
    this->lengthWidget.Configure16("Num", 64);
}

//------------------------------------------------------------------------------
bool
DisasmWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        this->drawMainContent(emu, this->startWidget.Get16(), this->lengthWidget.Get16());
        ImGui::Separator();
        this->drawControls();
    }
    ImGui::End();
    return this->Visible;
}

//------------------------------------------------------------------------------
void
DisasmWindow::drawMainContent(const yakc& emu, uword start_addr, int num_lines) {
    // this is a modified version of ImGuiMemoryEditor.h
    ImGui::BeginChild("##scrolling", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));

    const float line_height = ImGui::GetTextLineHeight();
    const float glyph_width = ImGui::CalcTextSize("F").x;
    const float cell_width = glyph_width * 3;
    ImGuiListClipper clipper(num_lines, line_height);

    // skip hidden line
    Disasm disasm;
    uword cur_addr = start_addr;
    for (int line_i = 0; (line_i < clipper.DisplayStart) && (line_i < num_lines); line_i++) {
        cur_addr += disasm.Disassemble(emu, cur_addr);
    }

    // display only visible items
    for (int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++) {
        const uword num_bytes = disasm.Disassemble(emu, cur_addr);

        // draw the address
        ImGui::Text("%04X: ", cur_addr);
        ImGui::SameLine();

        // print instruction bytes
        float line_start_x = ImGui::GetCursorPosX();
        for (int n = 0; n < num_bytes; n++) {
            ImGui::SameLine(line_start_x + cell_width * n);
            ImGui::Text("%02X ", emu.board.cpu.mem.r8(cur_addr++));
        }

        // print disassembled instruction
        ImGui::SameLine(line_start_x + cell_width * 4 + glyph_width * 2);
        ImGui::Text("%s", disasm.Result());
    }
    clipper.End();
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
}

//------------------------------------------------------------------------------
void
DisasmWindow::drawControls() {
    this->startWidget.Draw();
    ImGui::SameLine();
    this->lengthWidget.Draw();
}

} // namespace YAKC
