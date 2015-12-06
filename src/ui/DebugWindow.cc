//------------------------------------------------------------------------------
//  DebugWindow.cc
//------------------------------------------------------------------------------
#include "DebugWindow.h"
#include "IMUI/IMUI.h"
#include "UI.h"
#include "Disasm.h"

OryolClassImpl(DebugWindow);

using namespace Oryol;
using namespace yakc;

static const z80dbg::reg regs16[] = {
    z80dbg::AF, z80dbg::BC, z80dbg::DE, z80dbg::HL,
    z80dbg::AF_, z80dbg::BC_, z80dbg::DE_, z80dbg::HL_,
    z80dbg::IX, z80dbg::IY, z80dbg::SP, z80dbg::PC
};
static const z80dbg::reg regs8[] = {
    z80dbg::I, z80dbg::R, z80dbg::IM
};

//------------------------------------------------------------------------------
void
DebugWindow::Setup(kc85& kc) {
    this->setName("Debugger");

    // setup register table widgets
    for (z80dbg::reg r : regs16) {
        this->regWidget[r].Configure16(z80dbg::reg_name(r), z80dbg::get16(kc.cpu, r));
    }
    for (z80dbg::reg r : regs8) {
        this->regWidget[r].Configure8(z80dbg::reg_name(r), z80dbg::get8(kc.cpu, r));
    }
    this->breakPointWidget.Configure16("##bp", 0xFFFF);
}

//------------------------------------------------------------------------------
bool
DebugWindow::Draw(kc85& kc) {
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        this->drawRegisterTable(kc);
        ImGui::Separator();
        this->drawMainContent(kc, kc.cpu.state.PC, 48);
        ImGui::Separator();
        this->drawControls(kc);
    }
    ImGui::End();
    return this->Visible;
}

//------------------------------------------------------------------------------
void
DebugWindow::drawReg16(kc85& kc, z80dbg::reg r) {
    if (this->regWidget[r].Draw()) {
        z80dbg::set16(kc.cpu, r, this->regWidget[r].Get16());
    }
    else {
        this->regWidget[r].Set16(z80dbg::get16(kc.cpu, r));
    }
}

//------------------------------------------------------------------------------
void
DebugWindow::drawReg8(kc85& kc, z80dbg::reg r) {
    if (this->regWidget[r].Draw()) {
        z80dbg::set8(kc.cpu, r, this->regWidget[r].Get8());
    }
    else {
        this->regWidget[r].Set8(z80dbg::get8(kc.cpu, r));
    }
}

//------------------------------------------------------------------------------
void
DebugWindow::drawRegisterTable(kc85& kc) {
    const ImVec4 red(1.0f, 0.0f, 0.0f, 1.0f);
    const ImVec4 green(0.0f, 1.0f, 0.0f, 1.0f);

    ImGui::PushItemWidth(32);

    this->drawReg16(kc, z80dbg::AF); ImGui::SameLine(1 * 72);
    this->drawReg16(kc, z80dbg::BC); ImGui::SameLine(2 * 72);
    this->drawReg16(kc, z80dbg::DE); ImGui::SameLine(3 * 72);
    this->drawReg16(kc, z80dbg::HL); ImGui::SameLine(4 * 72);
    ImGui::PushItemWidth(16);
    this->drawReg8(kc, z80dbg::I); ImGui::SameLine(4 * 72 + 48);
    ImGui::TextColored(kc.cpu.state.IFF1 ? green:red, "IFF1");
    ImGui::PopItemWidth();

    this->drawReg16(kc, z80dbg::AF_); ImGui::SameLine(1 * 72);
    this->drawReg16(kc, z80dbg::BC_); ImGui::SameLine(2 * 72);
    this->drawReg16(kc, z80dbg::DE_); ImGui::SameLine(3 * 72);
    this->drawReg16(kc, z80dbg::HL_); ImGui::SameLine(4 * 72);
    ImGui::PushItemWidth(16);
    this->drawReg8(kc, z80dbg::IM); ImGui::SameLine(4 * 72 + 48);
    ImGui::TextColored(kc.cpu.state.IFF2 ? green:red, "IFF2");
    ImGui::PopItemWidth();

    this->drawReg16(kc, z80dbg::IX); ImGui::SameLine(1 * 72);
    this->drawReg16(kc, z80dbg::IY); ImGui::SameLine(2 * 72);
    this->drawReg16(kc, z80dbg::SP); ImGui::SameLine(3 * 72);
    this->drawReg16(kc, z80dbg::PC); ImGui::SameLine(4 * 72);
    ImGui::PushItemWidth(16);
    this->drawReg8(kc, z80dbg::R); ImGui::SameLine(4 * 72 + 48);
    ImGui::TextColored(kc.cpu.state.HALT ? green:red, "HALT");
    ImGui::PopItemWidth();

    char strFlags[9];
    const ubyte f = kc.cpu.state.F;
    strFlags[0] = (f & z80::SF) ? 'S':'-';
    strFlags[1] = (f & z80::ZF) ? 'Z':'-';
    strFlags[2] = (f & z80::YF) ? 'Y':'-';
    strFlags[3] = (f & z80::HF) ? 'H':'-';
    strFlags[4] = (f & z80::XF) ? 'X':'-';
    strFlags[5] = (f & z80::VF) ? 'V':'-';
    strFlags[6] = (f & z80::NF) ? 'N':'-';
    strFlags[7] = (f & z80::CF) ? 'C':'-';
    strFlags[8] = 0;
    ImGui::Text("flags: %s", strFlags);
    if (kc.cpu.state.INV)
    ImGui::PopItemWidth();
}

//------------------------------------------------------------------------------
void
DebugWindow::drawControls(kc85& kc) {
    ImGui::PushItemWidth(32);
    if (this->breakPointWidget.Draw()) {
        const uword bp_addr = this->breakPointWidget.Get16();
        if (bp_addr != 0xFFFF) {
            kc.dbg.enable_breakpoint(0, this->breakPointWidget.Get16());
        }
        else {
            kc.dbg.disable_breakpoint(0);
        }
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::Checkbox("break", &kc.paused);
    if (kc.paused) {
        ImGui::SameLine();
        if (ImGui::Button("step")) {
            kc.dbg.step_pc_modified(kc.cpu);
        }
    }
    ImGui::Checkbox("break on invalid opcode", &kc.cpu.break_on_invalid_opcode);
}

//------------------------------------------------------------------------------
void
DebugWindow::drawMainContent(kc85& kc, uword start_addr, int num_lines) {
    // this is a modified version of ImGuiMemoryEditor.h
    ImGui::BeginChild("##scrolling", ImVec2(0, -2 * ImGui::GetItemsLineHeightWithSpacing()));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1,1));

    const float line_height = ImGui::GetTextLineHeight();
    const int line_total_count = num_lines;
    const float glyph_width = ImGui::CalcTextSize("F").x;
    const float cell_width = glyph_width * 3; // "FF " we include trailing space in the width to easily catch clicks everywhere
    ImGuiListClipper clipper(line_total_count, line_height);

    Disasm disasm;
    uword cur_addr = start_addr;

    // set cur_addr to start of displayed region
    for (int line_i = z80dbg::pc_history_size; line_i < clipper.DisplayStart; line_i++) {
        cur_addr += disasm.Disassemble(kc, cur_addr);
    }

    // display only visible items
    for (int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++) {
        uword display_addr, num_bytes;
        if (line_i < z80dbg::pc_history_size) {
            display_addr = kc.dbg.get_pc_history(line_i);
            num_bytes = disasm.Disassemble(kc, display_addr);
            if (kc.dbg.is_breakpoint(display_addr)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 1.0f, 1.0f));
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
            }
        }
        else {
            display_addr = cur_addr;
            num_bytes = disasm.Disassemble(kc, display_addr);
                if ((cur_addr == start_addr) && kc.cpu.state.INV) {
                    // invalid/non-implemented opcode hit
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                }
                else if (kc.dbg.is_breakpoint(cur_addr)) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 1.0f, 1.0f));
                }
                else if (cur_addr == start_addr) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_Text, UI::ColorText);
                }
            cur_addr += num_bytes;
        }

        // set breakpoint
        ImGui::PushID(line_i);
        if (ImGui::Button(" B ")) {
            kc.dbg.toggle_breakpoint(0, display_addr);
            this->breakPointWidget.Set16(display_addr);
        }
        ImGui::PopID();
        ImGui::SameLine(32);

        // draw the address
        ImGui::Text("%04X: ", display_addr);
        ImGui::SameLine();

        // print instruction bytes
        float line_start_x = ImGui::GetCursorPosX();
        for (int n = 0; n < num_bytes; n++) {
            ImGui::SameLine(line_start_x + cell_width * n);
            ImGui::Text("%02X ", kc.cpu.mem.r8(display_addr++));
        }

        // print disassembled instruction
        ImGui::SameLine(line_start_x + cell_width * 4 + glyph_width * 2);
        ImGui::Text(disasm.Result());
        ImGui::PopStyleColor();
    }
    clipper.End();
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
}
