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

static const z80::reg regs16[] = {
    z80::rAF, z80::rBC, z80::rDE, z80::rHL,
    z80::rAF_, z80::rBC_, z80::rDE_, z80::rHL_,
    z80::rIX, z80::rIY, z80::rSP, z80::rPC
};
static const z80::reg regs8[] = {
    z80::rI, z80::rR, z80::rIM
};

//------------------------------------------------------------------------------
void
DebugWindow::Setup(const kc85& kc) {
    this->setName("Debugger");

    // setup register table widgets
    for (z80::reg r : regs16) {
        this->regWidget[r].Configure16(z80::reg_name(r), kc.cpu.get16(r));
    }
    for (z80::reg r : regs8) {
        this->regWidget[r].Configure8(z80::reg_name(r), kc.cpu.get8(r));
    }
    this->breakPointWidget.Configure16("##bp", 0xFFFF);
}

//------------------------------------------------------------------------------
bool
DebugWindow::Draw(kc85& kc) {
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
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
DebugWindow::drawReg16(kc85& kc, z80::reg r) {
    if (this->regWidget[r].Draw()) {
        kc.cpu.set16(r, this->regWidget[r].Get16());
    }
    else {
        this->regWidget[r].Set16(kc.cpu.get16(r));
    }
}

//------------------------------------------------------------------------------
void
DebugWindow::drawReg8(kc85& kc, z80::reg r) {
    if (this->regWidget[r].Draw()) {
        kc.cpu.set8(r, this->regWidget[r].Get8());
    }
    else {
        this->regWidget[r].Set8(kc.cpu.get8(r));
    }
}

//------------------------------------------------------------------------------
void
DebugWindow::drawRegisterTable(kc85& kc) {
    const ImVec4 red(1.0f, 0.0f, 0.0f, 1.0f);
    const ImVec4 green(0.0f, 1.0f, 0.0f, 1.0f);

    ImGui::PushItemWidth(32);

    this->drawReg16(kc, z80::rAF); ImGui::SameLine(1 * 72);
    this->drawReg16(kc, z80::rBC); ImGui::SameLine(2 * 72);
    this->drawReg16(kc, z80::rDE); ImGui::SameLine(3 * 72);
    this->drawReg16(kc, z80::rHL); ImGui::SameLine(4 * 72);
    ImGui::PushItemWidth(16);
    this->drawReg8(kc, z80::rI); ImGui::SameLine(4 * 72 + 48);
    this->drawReg8(kc, z80::rIM);
    ImGui::PopItemWidth();

    this->drawReg16(kc, z80::rAF_); ImGui::SameLine(1 * 72);
    this->drawReg16(kc, z80::rBC_); ImGui::SameLine(2 * 72);
    this->drawReg16(kc, z80::rDE_); ImGui::SameLine(3 * 72);
    this->drawReg16(kc, z80::rHL_); ImGui::SameLine(4 * 72);
    ImGui::PushItemWidth(16);
    this->drawReg8(kc, z80::rR); ImGui::SameLine(4 * 72 + 64);
    ImGui::TextColored(kc.cpu.state.IFF1 ? green:red, "IFF1");
    ImGui::PopItemWidth();

    this->drawReg16(kc, z80::rIX); ImGui::SameLine(1 * 72);
    this->drawReg16(kc, z80::rIY); ImGui::SameLine(2 * 72);
    this->drawReg16(kc, z80::rSP); ImGui::SameLine(3 * 72);
    this->drawReg16(kc, z80::rPC); ImGui::SameLine(4 * 72);
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
    ImGui::Text(strFlags); ImGui::SameLine(4 * 72 + 64);
    ImGui::TextColored(kc.cpu.state.IFF2 ? green:red, "IFF2");
    ImGui::PopItemWidth();
}

//------------------------------------------------------------------------------
void
DebugWindow::drawControls(kc85& kc) {
    ImGui::PushItemWidth(32);
    if (this->breakPointWidget.Draw()) {
        const uword bp_addr = this->breakPointWidget.Get16();
        if (bp_addr != 0xFFFF) {
            kc.breakpoint_enabled = true;
            kc.breakpoint_address = this->breakPointWidget.Get16();
        }
        else {
            kc.breakpoint_enabled = false;
        }
    }
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::Checkbox("break", &kc.paused);
    if (kc.paused) {
        ImGui::SameLine();
        if (ImGui::Button("step")) {
            kc.debug_step();
        }
    }
}

//------------------------------------------------------------------------------
void
DebugWindow::drawMainContent(const kc85& kc, uword start_addr, int num_lines) {
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
    for (int line_i = z80::pc_history_size; line_i < clipper.DisplayStart; line_i++) {
        cur_addr += disasm.Disassemble(kc, cur_addr);
    }

    // display only visible items
    for (int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++) {
        uword display_addr, num_bytes;
        if (line_i < z80::pc_history_size) {
            display_addr = kc.cpu.get_pc_history(line_i);
            num_bytes = disasm.Disassemble(kc, display_addr);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
        }
        else {
            display_addr = cur_addr;
            num_bytes = disasm.Disassemble(kc, display_addr);
            if (cur_addr == start_addr) {
                if (kc.cpu.state.INV) {
                    // invalid/non-implemented opcode hit
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                }
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, UI::ColorText);
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

        // print disassembled instruction
        ImGui::SameLine(line_start_x + cell_width * 4 + glyph_width * 2);
        ImGui::Text(disasm.Result());
        ImGui::PopStyleColor();
    }
    clipper.End();
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
}