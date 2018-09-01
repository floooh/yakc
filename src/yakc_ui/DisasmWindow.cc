//------------------------------------------------------------------------------
//  DisasmWindow.cc
//------------------------------------------------------------------------------
#include "DisasmWindow.h"
#include "IMUI/IMUI.h"
#include "Util.h"
#include "Disasm.h"
#include "yakc/util/breadboard.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
DisasmWindow::Setup(yakc& emu) {
    this->setName("Disassembler");
}

//------------------------------------------------------------------------------
bool
DisasmWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        if (board.mem) {
            this->drawMainContent(emu, this->startAddr, this->numLines);
            ImGui::Separator();
            this->drawControls();
        }
    }
    ImGui::End();
    return this->Visible;
}

//------------------------------------------------------------------------------
void
DisasmWindow::drawMainContent(const yakc& emu, uint16_t start_addr, int num_lines) {
    YAKC_ASSERT(board.mem);

    // this is a modified version of ImGuiMemoryEditor.h
    ImGui::BeginChild("##scrolling", ImVec2(0, -(4 + ImGui::GetFrameHeightWithSpacing())));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));

    const float line_height = ImGui::GetTextLineHeight();
    const float glyph_width = ImGui::CalcTextSize("F").x;
    const float cell_width = glyph_width * 3;
    ImGuiListClipper clipper(num_lines, line_height);

    // skip hidden line
    Disasm disasm;
    uint16_t cur_addr = start_addr;
    for (int line_i = 0; (line_i < clipper.DisplayStart) && (line_i < num_lines); line_i++) {
        cur_addr += disasm.Disassemble(emu, cur_addr);
    }

    // display only visible items
    for (int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++) {
        const uint16_t num_bytes = disasm.Disassemble(emu, cur_addr);

        // draw the address
        ImGui::Text("%04X: ", cur_addr);
        ImGui::SameLine();

        // print instruction bytes
        float line_start_x = ImGui::GetCursorPosX();
        for (int n = 0; n < num_bytes; n++) {
            ImGui::SameLine(line_start_x + cell_width * n);
            uint8_t val = mem_rd(board.mem, cur_addr++);
            ImGui::Text("%02X ", val);
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
    Util::InputHex16("Start", this->startAddr);
    ImGui::SameLine();
    Util::InputHex16("Lines", this->numLines);
}

} // namespace YAKC
