//------------------------------------------------------------------------------
//  DisasmWindow.cc
//------------------------------------------------------------------------------
#include "DisasmWindow.h"
#include "IMUI/IMUI.h"
#include "Disasm.h"

OryolClassImpl(DisasmWindow);

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
DisasmWindow::Setup(const kc85& kc) {
    this->setName("Disassembler");
    this->history.Reserve(max_history_size);
}

//------------------------------------------------------------------------------
bool
DisasmWindow::Draw(kc85& kc) {
    ImGui::SetNextWindowSize(ImVec2(280, 220), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        const uword start_addr = kc.cpu.state.PC;
        this->drawContent(kc, start_addr, 48);
        this->updateHistory(kc, start_addr);
    }
    ImGui::End();
    return this->Visible;
}

//------------------------------------------------------------------------------
void
DisasmWindow::updateHistory(const kc85& kc, uword addr) {
    if (kc.paused()) {
        if (this->history.Empty() || (this->history.Back() != addr)) {
            this->history.Add(addr);
        }
        if (this->history.Size() > max_history_size) {
            this->history.Erase(0);
        }
    }
    else {
        if (!this->history.Empty()) {
            this->history.Clear();
        }
    }
}

//------------------------------------------------------------------------------
void
DisasmWindow::drawContent(const kc85& kc, uword start_addr, int num_lines) {
    // this is a modified version of ImGuiMemoryEditor.h
    ImGui::BeginChild("##scrolling", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));

    const float line_height = ImGui::GetTextLineHeight();
    const int line_total_count = num_lines;
    const float glyph_width = ImGui::CalcTextSize("F").x;
    const float cell_width = glyph_width * 3; // "FF " we include trailing space in the width to easily catch clicks everywhere
    ImGuiListClipper clipper(line_total_count, line_height);

    Disasm disasm;
    uword cur_addr = start_addr;

    // set cur_addr to start of displayed region
    for (int line_i = this->history.Size(); line_i < clipper.DisplayStart; line_i++) {
        cur_addr += disasm.Disassemble(kc, cur_addr);
    }

    // display only visible items
    uword next_instr_addr = 0xffff;
    for (int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++) {
        uword display_addr, num_bytes;
        if (!this->history.Empty() && (line_i < this->history.Size()-1)) {
            display_addr = this->history[line_i];
            num_bytes = disasm.Disassemble(kc, display_addr);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5));
        }
        else {
            display_addr = cur_addr;
            num_bytes = disasm.Disassemble(kc, display_addr);
            if (cur_addr == start_addr) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0));
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0));
            }
            cur_addr += num_bytes;
        }

        // draw the address
        ImGui::Text("%04X: ", display_addr);
        ImGui::SameLine();

        // print instruction bytes
        float line_start_x = ImGui::GetCursorPosX();
        for (int n = 0; n < num_bytes; n++) {
            ImGui::SameLine(line_start_x + cell_width * n);
            ImGui::Text("%02X ", kc.cpu.mem.r8(display_addr++));
        }
        next_instr_addr = display_addr;

        // print disassembled instruction
        ImGui::SameLine(line_start_x + cell_width * 4 + glyph_width * 2);
        ImGui::Text(disasm.Result());
        ImGui::PopStyleColor();
    }
    clipper.End();
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
}
