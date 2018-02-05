//------------------------------------------------------------------------------
//  DebugWindow.cc
//------------------------------------------------------------------------------
#include "DebugWindow.h"
#include "IMUI/IMUI.h"
#include "yakc_ui/UI.h"
#include "Disasm.h"
#include "Util.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
DebugWindow::Setup(yakc& emu) {
    this->setName("Debugger");
}

//------------------------------------------------------------------------------
bool
DebugWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(460, 400), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        if (emu.cpu_type() == cpu_model::z80) {
            this->drawZ80RegisterTable(emu);
            ImGui::Separator();
            this->drawMainContent(emu, emu.board.z80.PC, 48);
            ImGui::Separator();
        }
        else {
            this->draw6502RegisterTable(emu);
            ImGui::Separator();
            this->drawMainContent(emu, emu.board.mos6502.PC, 48);
            ImGui::Separator();
        }
        this->drawControls(emu);
    }
    ImGui::End();
    return this->Visible;
}

//------------------------------------------------------------------------------
void
DebugWindow::drawZ80RegisterTable(yakc& emu) {
    const ImVec4 red = UI::DisabledColor;
    const ImVec4 green = UI::EnabledColor;

    auto& cpu = emu.board.z80;
    Util::InputHex16("AF", cpu.AF); ImGui::SameLine(1 * 72);
    Util::InputHex16("BC", cpu.BC); ImGui::SameLine(2 * 72);
    Util::InputHex16("DE", cpu.DE); ImGui::SameLine(3 * 72);
    Util::InputHex16("HL", cpu.HL); ImGui::SameLine(4 * 72);
    Util::InputHex16("WZ", cpu.WZ); ImGui::SameLine(5 * 72);
    Util::InputHex8("I", cpu.I); ImGui::SameLine(5 * 72 + 48);
    ImGui::TextColored(emu.board.z80.IFF1 ? green:red, "IFF1");

    Util::InputHex16("AF'", cpu.AF_); ImGui::SameLine(1 * 72);
    Util::InputHex16("BC'", cpu.BC_); ImGui::SameLine(2 * 72);
    Util::InputHex16("DE'", cpu.DE_); ImGui::SameLine(3 * 72);
    Util::InputHex16("HL'", cpu.HL_); ImGui::SameLine(4 * 72);
    Util::InputHex16("WZ'", cpu.WZ_); ImGui::SameLine(5 * 72);
    Util::InputHex8("IM", cpu.IM); ImGui::SameLine(5 * 72 + 48);
    ImGui::TextColored(emu.board.z80.IFF2 ? green:red, "IFF2");

    Util::InputHex16("IX", cpu.IX); ImGui::SameLine(1 * 72);
    Util::InputHex16("IY", cpu.IY); ImGui::SameLine(2 * 72);
    Util::InputHex16("SP", cpu.SP); ImGui::SameLine(3 * 72);
    Util::InputHex16("PC", cpu.PC); ImGui::SameLine(4 * 72);
    Util::InputHex8("R", cpu.R); ImGui::SameLine();

    char strFlags[9];
    const uint8_t f = emu.board.z80.F;
    strFlags[0] = (f & z80::SF) ? 'S':'-';
    strFlags[1] = (f & z80::ZF) ? 'Z':'-';
    strFlags[2] = (f & z80::YF) ? 'Y':'-';
    strFlags[3] = (f & z80::HF) ? 'H':'-';
    strFlags[4] = (f & z80::XF) ? 'X':'-';
    strFlags[5] = (f & z80::VF) ? 'V':'-';
    strFlags[6] = (f & z80::NF) ? 'N':'-';
    strFlags[7] = (f & z80::CF) ? 'C':'-';
    strFlags[8] = 0;
    ImGui::Text(" %s ", strFlags);
    ImGui::SameLine(5 * 72 + 48);
    ImGui::TextColored(emu.board.z80.HALT ? green:red, "HALT");
}

//------------------------------------------------------------------------------
void
DebugWindow::draw6502RegisterTable(yakc& emu) {

    auto& cpu = emu.board.mos6502;
    Util::InputHex8("A", cpu.A); ImGui::SameLine(1 * 48 + 4);
    Util::InputHex8("X", cpu.X); ImGui::SameLine(2 * 48);
    Util::InputHex8("Y", cpu.Y); ImGui::SameLine(3 * 48);
    Util::InputHex8("S", cpu.S); ImGui::SameLine(4 * 48);
    Util::InputHex8("P", cpu.P); ImGui::SameLine(5 * 48);
    Util::InputHex16("PC", cpu.PC); ImGui::SameLine(6 * 46 + 20);

    char strFlags[9];
    const uint8_t f = emu.board.mos6502.P;
    strFlags[0] = (f & mos6502::NF) ? 'N':'-';
    strFlags[1] = (f & mos6502::VF) ? 'V':'-';
    strFlags[2] = (f & mos6502::XF) ? 'x':'-';
    strFlags[3] = (f & mos6502::BF) ? 'B':'-';
    strFlags[4] = (f & mos6502::DF) ? 'D':'-';
    strFlags[5] = (f & mos6502::IF) ? 'I':'-';
    strFlags[6] = (f & mos6502::ZF) ? 'Z':'-';
    strFlags[7] = (f & mos6502::CF) ? 'C':'-';
    strFlags[8] = 0;
    ImGui::Text(" %s ", strFlags);
}

//------------------------------------------------------------------------------
void
DebugWindow::drawControls(yakc& emu) {
    uint16_t bp_addr = 0xFFFF;
    if (emu.board.dbg.breakpoint_enabled()) {
        bp_addr = emu.board.dbg.breakpoint_addr();
    }
    if (Util::InputHex16("", bp_addr)) {
        if (bp_addr != 0xFFFF) {
            emu.board.dbg.enable_breakpoint(bp_addr);
        }
        else {
            emu.board.dbg.disable_breakpoint();
        }
    }
    ImGui::SameLine();
    ImGui::Checkbox("break", &emu.board.dbg.active);
    if (emu.board.dbg.active) {
        ImGui::SameLine();
        if (ImGui::Button("step")) {
            emu.step_debug();
        }
    }
    ImGui::Checkbox("break on invalid opcode", &emu.board.z80.break_on_invalid_opcode);
}

//------------------------------------------------------------------------------
void
DebugWindow::drawMainContent(yakc& emu, uint16_t start_addr, int num_lines) {
    // this is a modified version of ImGuiMemoryEditor.h
    ImGui::BeginChild("##scrolling", ImVec2(0, -2 * (ImGui::GetFrameHeightWithSpacing()+2)));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1,1));

    const float line_height = ImGui::GetTextLineHeight();
    const int line_total_count = num_lines;
    const float glyph_width = ImGui::CalcTextSize("F").x;
    const float cell_width = glyph_width * 3; // "FF " we include trailing space in the width to easily catch clicks everywhere
    ImGuiListClipper clipper(line_total_count, line_height);

    Disasm disasm;
    uint16_t cur_addr = start_addr;

    // set cur_addr to start of displayed region
    for (int line_i = cpudbg::history_size; line_i < clipper.DisplayStart; line_i++) {
        cur_addr += disasm.Disassemble(emu, cur_addr);
    }

    // display only visible items
    for (int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++) {
        uint16_t op_addr, op_cycles, num_bytes;
        if (line_i < cpudbg::history_size) {
            auto hist_item = emu.board.dbg.get_pc_history(line_i);
            op_addr = hist_item.pc;
            op_cycles = hist_item.cycles;
            num_bytes = disasm.Disassemble(emu, hist_item.pc);
            if (emu.board.dbg.is_breakpoint(hist_item.pc)) {
                ImGui::PushStyleColor(ImGuiCol_Text, UI::EnabledBreakpointColor);
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, UI::DisabledBreakpointColor);
            }
        }
        else {
            op_addr = cur_addr;
            op_cycles = 0;
            num_bytes = disasm.Disassemble(emu, op_addr);
            bool inv = emu.cpu_type() == cpu_model::z80 ? emu.board.z80.INV : false;
            if ((cur_addr == start_addr) && inv) {
                // invalid/non-implemented opcode hit
                ImGui::PushStyleColor(ImGuiCol_Text, UI::InvalidOpCodeColor);
            }
            else if (emu.board.dbg.is_breakpoint(cur_addr)) {
                ImGui::PushStyleColor(ImGuiCol_Text, UI::EnabledBreakpointColor);
            }
            else if (cur_addr == start_addr) {
                ImGui::PushStyleColor(ImGuiCol_Text, UI::EnabledColor);
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, UI::DefaultTextColor);
            }
            cur_addr += num_bytes;
        }

        // wrap current PC into 2 separator lines
        if ((line_i == cpudbg::history_size) || (line_i == cpudbg::history_size+1)) {
            ImGui::Separator();
        }

        // set breakpoint
        ImGui::PushID(line_i);
        if (ImGui::Button(" B ")) {
            emu.board.dbg.toggle_breakpoint(op_addr);
        }
        ImGui::PopID();
        ImGui::SameLine(32);

        // draw the address
        ImGui::Text("%04X: ", op_addr);
        ImGui::SameLine();

        // print instruction bytes
        float line_start_x = ImGui::GetCursorPosX();
        for (int n = 0; n < num_bytes; n++) {
            ImGui::SameLine(line_start_x + cell_width * n);
            if (emu.cpu_type() == cpu_model::z80) {
                ImGui::Text("%02X ", emu.board.z80.mem.r8(op_addr++));
            }
            else {
                ImGui::Text("%02X ", emu.board.mos6502.mem.r8(op_addr++));
            }
        }

        // print disassembled instruction
        float offset = line_start_x + cell_width * 4 + glyph_width * 2;
        ImGui::SameLine(offset);
        ImGui::Text("%s", disasm.Result());
        if (op_cycles > 0) {
            offset += glyph_width * 24;
            ImGui::SameLine(offset);
            ImGui::Text("%d", op_cycles);
        }
        ImGui::PopStyleColor();
    }
    clipper.End();
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
}

} // namespace YAKC
