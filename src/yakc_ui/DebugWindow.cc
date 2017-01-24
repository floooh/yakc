//------------------------------------------------------------------------------
//  DebugWindow.cc
//------------------------------------------------------------------------------
#include "DebugWindow.h"
#include "IMUI/IMUI.h"
#include "yakc_ui/UI.h"
#include "Disasm.h"

using namespace Oryol;

namespace YAKC {

static const cpudbg::z80reg z80regs16[] = {
    cpudbg::z80reg::AF,
    cpudbg::z80reg::BC,
    cpudbg::z80reg::DE,
    cpudbg::z80reg::HL,
    cpudbg::z80reg::WZ,
    cpudbg::z80reg::AF_,
    cpudbg::z80reg::BC_,
    cpudbg::z80reg::DE_,
    cpudbg::z80reg::HL_,
    cpudbg::z80reg::WZ_,
    cpudbg::z80reg::IX,
    cpudbg::z80reg::IY,
    cpudbg::z80reg::SP,
    cpudbg::z80reg::PC
};
static const cpudbg::z80reg z80regs8[] = {
    cpudbg::z80reg::I, cpudbg::z80reg::R, cpudbg::z80reg::IM
};

static const cpudbg::m6502reg m6502regs8[] = {
    cpudbg::m6502reg::A,
    cpudbg::m6502reg::X,
    cpudbg::m6502reg::Y,
    cpudbg::m6502reg::S,
    cpudbg::m6502reg::P,
};
static const cpudbg::m6502reg m6502regs16[] = {
    cpudbg::m6502reg::PC,
};

//------------------------------------------------------------------------------
void
DebugWindow::Setup(yakc& emu) {
    this->setName("Debugger");

    // setup register table widgets
    for (cpudbg::z80reg r : z80regs16) {
        this->z80RegWidget[int(r)].Configure16(cpudbg::reg_name(r), cpudbg::get16(emu.board.z80, r));
    }
    for (cpudbg::z80reg r : z80regs8) {
        this->z80RegWidget[int(r)].Configure8(cpudbg::reg_name(r), cpudbg::get8(emu.board.z80, r));
    }
    for (cpudbg::m6502reg r : m6502regs16) {
        this->m6502RegWidget[int(r)].Configure16(cpudbg::reg_name(r), cpudbg::get16(emu.board.mos6502, r));
    }
    for (cpudbg::m6502reg r : m6502regs8) {
        this->m6502RegWidget[int(r)].Configure8(cpudbg::reg_name(r), cpudbg::get8(emu.board.mos6502, r));
    }
    this->breakPointWidget.Configure16("##bp", 0xFFFF);
}

//------------------------------------------------------------------------------
bool
DebugWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(460, 400), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
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
DebugWindow::drawZ80Reg16(yakc& emu, cpudbg::z80reg r) {
    if (this->z80RegWidget[int(r)].Draw()) {
        cpudbg::set16(emu.board.z80, r, this->z80RegWidget[int(r)].Get16());
    }
    else {
        this->z80RegWidget[int(r)].Set16(cpudbg::get16(emu.board.z80, r));
    }
}

//------------------------------------------------------------------------------
void
DebugWindow::drawZ80Reg8(yakc& emu, cpudbg::z80reg r) {
    if (this->z80RegWidget[int(r)].Draw()) {
        cpudbg::set8(emu.board.z80, r, this->z80RegWidget[int(r)].Get8());
    }
    else {
        this->z80RegWidget[int(r)].Set8(cpudbg::get8(emu.board.z80, r));
    }
}

//------------------------------------------------------------------------------
void
DebugWindow::drawZ80RegisterTable(yakc& emu) {
    const ImVec4 red = UI::DisabledColor;
    const ImVec4 green = UI::EnabledColor;

    this->drawZ80Reg16(emu, cpudbg::z80reg::AF); ImGui::SameLine(1 * 72);
    this->drawZ80Reg16(emu, cpudbg::z80reg::BC); ImGui::SameLine(2 * 72);
    this->drawZ80Reg16(emu, cpudbg::z80reg::DE); ImGui::SameLine(3 * 72);
    this->drawZ80Reg16(emu, cpudbg::z80reg::HL); ImGui::SameLine(4 * 72);
    this->drawZ80Reg16(emu, cpudbg::z80reg::WZ); ImGui::SameLine(5 * 72);
    this->drawZ80Reg8(emu, cpudbg::z80reg::I); ImGui::SameLine(5 * 72 + 48);
    ImGui::TextColored(emu.board.z80.IFF1 ? green:red, "IFF1");

    this->drawZ80Reg16(emu, cpudbg::z80reg::AF_); ImGui::SameLine(1 * 72);
    this->drawZ80Reg16(emu, cpudbg::z80reg::BC_); ImGui::SameLine(2 * 72);
    this->drawZ80Reg16(emu, cpudbg::z80reg::DE_); ImGui::SameLine(3 * 72);
    this->drawZ80Reg16(emu, cpudbg::z80reg::HL_); ImGui::SameLine(4 * 72);
    this->drawZ80Reg16(emu, cpudbg::z80reg::WZ_); ImGui::SameLine(5 * 72);
    
    this->drawZ80Reg8(emu, cpudbg::z80reg::IM); ImGui::SameLine(5 * 72 + 48);
    ImGui::TextColored(emu.board.z80.IFF2 ? green:red, "IFF2");

    this->drawZ80Reg16(emu, cpudbg::z80reg::IX); ImGui::SameLine(1 * 72);
    this->drawZ80Reg16(emu, cpudbg::z80reg::IY); ImGui::SameLine(2 * 72);
    this->drawZ80Reg16(emu, cpudbg::z80reg::SP); ImGui::SameLine(3 * 72);
    this->drawZ80Reg16(emu, cpudbg::z80reg::PC); ImGui::SameLine(4 * 72);
    this->drawZ80Reg8(emu, cpudbg::z80reg::R); ImGui::SameLine();

    char strFlags[9];
    const ubyte f = emu.board.z80.F;
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
DebugWindow::draw6502Reg16(yakc& emu, cpudbg::m6502reg r) {
    if (this->m6502RegWidget[int(r)].Draw()) {
        cpudbg::set16(emu.board.mos6502, r, this->m6502RegWidget[int(r)].Get16());
    }
    else {
        this->m6502RegWidget[int(r)].Set16(cpudbg::get16(emu.board.mos6502, r));
    }
}

//------------------------------------------------------------------------------
void
DebugWindow::draw6502Reg8(yakc& emu, cpudbg::m6502reg r) {
    if (this->m6502RegWidget[int(r)].Draw()) {
        cpudbg::set8(emu.board.mos6502, r, this->m6502RegWidget[int(r)].Get8());
    }
    else {
        this->m6502RegWidget[int(r)].Set8(cpudbg::get8(emu.board.mos6502, r));
    }
}

//------------------------------------------------------------------------------
void
DebugWindow::draw6502RegisterTable(yakc& emu) {
    this->draw6502Reg8(emu, cpudbg::m6502reg::A); ImGui::SameLine(1 * 48 + 4);
    this->draw6502Reg8(emu, cpudbg::m6502reg::X); ImGui::SameLine(2 * 48);
    this->draw6502Reg8(emu, cpudbg::m6502reg::Y); ImGui::SameLine(3 * 48);
    this->draw6502Reg8(emu, cpudbg::m6502reg::S); ImGui::SameLine(4 * 48);
    this->draw6502Reg8(emu, cpudbg::m6502reg::P); ImGui::SameLine(5 * 48);
    this->draw6502Reg16(emu, cpudbg::m6502reg::PC); ImGui::SameLine(6 * 46 + 20);

    char strFlags[9];
    const ubyte f = emu.board.mos6502.P;
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
    if (emu.board.dbg.breakpoint_enabled(0)) {
        this->breakPointWidget.Set16(emu.board.dbg.breakpoint_addr(0));
    }
    else {
        this->breakPointWidget.Set16(0xFFFF);
    }
    if (this->breakPointWidget.Draw()) {
        const uword bp_addr = this->breakPointWidget.Get16();
        if (bp_addr != 0xFFFF) {
            emu.board.dbg.enable_breakpoint(0, this->breakPointWidget.Get16());
        }
        else {
            emu.board.dbg.disable_breakpoint(0);
        }
    }
    ImGui::SameLine();
    ImGui::Checkbox("break", &emu.board.dbg.paused);
    if (emu.board.dbg.paused) {
        ImGui::SameLine();
        if (ImGui::Button("step")) {
            if (emu.cpu_type() == cpu_model::z80) {
                emu.board.dbg.step_pc_modified(emu.get_bus(), emu.board.z80);
            }
            else {
                emu.board.dbg.step_pc_modified(emu.board.mos6502);
            }
        }
    }
    ImGui::Checkbox("break on invalid opcode", &emu.board.z80.break_on_invalid_opcode);
}

//------------------------------------------------------------------------------
void
DebugWindow::drawMainContent(yakc& emu, uword start_addr, int num_lines) {
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
    for (int line_i = cpudbg::pc_history_size; line_i < clipper.DisplayStart; line_i++) {
        cur_addr += disasm.Disassemble(emu, cur_addr);
    }

    // display only visible items
    for (int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++) {
        uword display_addr, num_bytes;
        if (line_i < cpudbg::pc_history_size) {
            display_addr = emu.board.dbg.get_pc_history(line_i);
            num_bytes = disasm.Disassemble(emu, display_addr);
            if (emu.board.dbg.is_breakpoint(display_addr)) {
                ImGui::PushStyleColor(ImGuiCol_Text, UI::EnabledBreakpointColor);
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, UI::DisabledBreakpointColor);
            }
        }
        else {
            display_addr = cur_addr;
            num_bytes = disasm.Disassemble(emu, display_addr);
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

        // set breakpoint
        ImGui::PushID(line_i);
        if (ImGui::Button(" B ")) {
            emu.board.dbg.toggle_breakpoint(0, display_addr);
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
            if (emu.cpu_type() == cpu_model::z80) {
                ImGui::Text("%02X ", emu.board.z80.mem.r8(display_addr++));
            }
            else {
                ImGui::Text("%02X ", emu.board.mos6502.mem.r8io(display_addr++));
            }
        }

        // print disassembled instruction
        ImGui::SameLine(line_start_x + cell_width * 4 + glyph_width * 2);
        ImGui::Text("%s", disasm.Result());
        ImGui::PopStyleColor();
    }
    clipper.End();
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
}

} // namespace YAKC
