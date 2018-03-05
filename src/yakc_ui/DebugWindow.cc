//------------------------------------------------------------------------------
//  DebugWindow.cc
//------------------------------------------------------------------------------
#include "DebugWindow.h"
#include "IMUI/IMUI.h"
#include "yakc_ui/UI.h"
#include "Disasm.h"
#include "Util.h"
#include "yakc/util/breadboard.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
DebugWindow::Setup(yakc& emu) {
    this->setName("Debugger");
    this->emu = &emu;
}

//------------------------------------------------------------------------------
bool
DebugWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(460, 400), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        if (emu.cpu_type() == cpu_model::z80) {
            this->drawZ80RegisterTable();
            ImGui::Separator();
            this->drawMainContent(emu, board.z80.PC, 48);
            ImGui::Separator();
        }
        else {
            this->draw6502RegisterTable();
            ImGui::Separator();
            this->drawMainContent(emu, board.m6502.state.PC, 48);
            ImGui::Separator();
        }
        this->drawControls(emu);
    }
    ImGui::End();
    return this->Visible;
}

//------------------------------------------------------------------------------
void
DebugWindow::drawZ80RegisterTable() {
    const ImVec4 red = UI::DisabledColor;
    const ImVec4 green = UI::EnabledColor;

    auto& cpu = board.z80;
    Util::InputHex16("AF", cpu.AF); ImGui::SameLine(1 * 72);
    Util::InputHex16("BC", cpu.BC); ImGui::SameLine(2 * 72);
    Util::InputHex16("DE", cpu.DE); ImGui::SameLine(3 * 72);
    Util::InputHex16("HL", cpu.HL); ImGui::SameLine(4 * 72);
    Util::InputHex16("WZ", cpu.WZ); ImGui::SameLine(5 * 72);
    Util::InputHex8("I", cpu.I); ImGui::SameLine(5 * 72 + 48);
    ImGui::TextColored(board.z80.IFF1 ? green:red, "IFF1");

    Util::InputHex16("AF'", cpu.AF_); ImGui::SameLine(1 * 72);
    Util::InputHex16("BC'", cpu.BC_); ImGui::SameLine(2 * 72);
    Util::InputHex16("DE'", cpu.DE_); ImGui::SameLine(3 * 72);
    Util::InputHex16("HL'", cpu.HL_); ImGui::SameLine(4 * 72);
    Util::InputHex8("IM", cpu.IM); ImGui::SameLine(5 * 72 + 48);
    ImGui::TextColored(board.z80.IFF2 ? green:red, "IFF2");

    Util::InputHex16("IX", cpu.IX); ImGui::SameLine(1 * 72);
    Util::InputHex16("IY", cpu.IY); ImGui::SameLine(2 * 72);
    Util::InputHex16("SP", cpu.SP); ImGui::SameLine(3 * 72);
    Util::InputHex16("PC", cpu.PC); ImGui::SameLine(4 * 72);
    Util::InputHex8("R", cpu.R); ImGui::SameLine();

    char strFlags[9];
    const uint8_t f = board.z80.F;
    strFlags[0] = (f & Z80_SF) ? 'S':'-';
    strFlags[1] = (f & Z80_ZF) ? 'Z':'-';
    strFlags[2] = (f & Z80_YF) ? 'Y':'-';
    strFlags[3] = (f & Z80_HF) ? 'H':'-';
    strFlags[4] = (f & Z80_XF) ? 'X':'-';
    strFlags[5] = (f & Z80_VF) ? 'V':'-';
    strFlags[6] = (f & Z80_NF) ? 'N':'-';
    strFlags[7] = (f & Z80_CF) ? 'C':'-';
    strFlags[8] = 0;
    ImGui::Text(" %s ", strFlags);
    ImGui::SameLine(5 * 72 + 48);
    ImGui::TextColored((board.z80.PINS & Z80_HALT) ? green:red, "HALT");
}

//------------------------------------------------------------------------------
void
DebugWindow::draw6502RegisterTable() {
    auto& cpu = board.m6502;
    Util::InputHex8("A", cpu.state.A); ImGui::SameLine(1 * 48 + 4);
    Util::InputHex8("X", cpu.state.X); ImGui::SameLine(2 * 48);
    Util::InputHex8("Y", cpu.state.Y); ImGui::SameLine(3 * 48);
    Util::InputHex8("S", cpu.state.S); ImGui::SameLine(4 * 48);
    Util::InputHex8("P", cpu.state.P); ImGui::SameLine(5 * 48);
    Util::InputHex16("PC", cpu.state.PC); ImGui::SameLine(6 * 46 + 20);

    char strFlags[9];
    const uint8_t f = board.m6502.state.P;
    strFlags[0] = (f & M6502_NF) ? 'N':'-';
    strFlags[1] = (f & M6502_VF) ? 'V':'-';
    strFlags[2] = (f & M6502_XF) ? 'x':'-';
    strFlags[3] = (f & M6502_BF) ? 'B':'-';
    strFlags[4] = (f & M6502_DF) ? 'D':'-';
    strFlags[5] = (f & M6502_IF) ? 'I':'-';
    strFlags[6] = (f & M6502_ZF) ? 'Z':'-';
    strFlags[7] = (f & M6502_CF) ? 'C':'-';
    strFlags[8] = 0;
    ImGui::Text(" %s ", strFlags);
}

//------------------------------------------------------------------------------
void
DebugWindow::drawControls(yakc& emu) {
    uint16_t bp_addr = 0xFFFF;
    bool bp_enabled = board.dbg.breakpoint_enabled();
    bp_addr = board.dbg.breakpoint_addr();
    if (ImGui::Checkbox("##breakpoint", &bp_enabled)) {
        if (bp_enabled) {
            board.dbg.set_breakpoint(bp_addr);
        }
        else {
            board.dbg.disable_breakpoint();
        }
    }
    if (ImGui::IsItemHovered()) { ImGui::SetTooltip("enable/disable breakpoint"); }
    ImGui::SameLine();
    if (Util::InputHex16("", bp_addr)) {
        board.dbg.set_breakpoint(bp_addr);
    }
    ImGui::SameLine();
    if (board.dbg.break_stopped()) {
        if (ImGui::Button("Cont")) {
            board.dbg.clear_history();
            board.dbg.break_continue();
        }
        ImGui::SameLine();
        if (ImGui::Button("Step")) {
            emu.step();
        }
        ImGui::SameLine();
        if (ImGui::Button(">Int")) {
            if (emu.cpu_type() == cpu_model::z80) {
                this->cpu_pins = board.z80.PINS;
            }
            else {
                this->cpu_pins = board.m6502.state.PINS;
            }
            emu.step_until([this](uint32_t ticks)->bool {
                bool triggered = false;
                if (this->emu->cpu_type() == cpu_model::z80) {
                    uint64_t cur_pins = board.z80.PINS;
                    triggered = 0 != (((cur_pins ^ this->cpu_pins) & cur_pins) & Z80_INT);
                    this->cpu_pins = cur_pins;
                }
                else {
                    uint64_t cur_pins = board.m6502.state.PINS;
                    triggered = 0 != (((cur_pins ^ this->cpu_pins) & cur_pins) & (M6502_IRQ|M6502_NMI));
                    this->cpu_pins = cur_pins;
                }
                return (ticks > 1000000) || triggered;
            });
        }
        if (ImGui::IsItemHovered()) { ImGui::SetTooltip("Run to next interrupt\n"); }
        ImGui::SameLine();
        // tint the framebuffer red, to visualize video decoding
        if (ImGui::Button("Tint")) {
            int w, h;
            uint32_t* ptr = (uint32_t*) emu.framebuffer(w, h);
            if (ptr) {
                for (int y = 0; y < h; y++) {
                    for (int x = 0; x < w; x++, ptr++) {
                        uint32_t c = *ptr;
                        uint8_t r = (uint8_t) c;
                        uint8_t g = (uint8_t) (c>>8);
                        uint8_t b = (uint8_t) (c>>16);
                        uint8_t a = (uint8_t) (c>>24);
                        r>>=2; g>>=2; b>>=2;
                        r|=0x80;
                        *ptr = (a<<24)|(b<<16)|(g<<8)|r;
                    }
                }
            }
        }
        if (ImGui::IsItemHovered()) { ImGui::SetTooltip("tint framebuffer content"); }
    }
    else {
        if (ImGui::Button("Stop")) {
            board.dbg.break_stop();
        }
    }
}

//------------------------------------------------------------------------------
void
DebugWindow::drawMainContent(yakc& emu, uint16_t start_addr, int num_lines) {
    ImGui::BeginChild("##scrolling", ImVec2(0, -1 * (ImGui::GetFrameHeightWithSpacing()+4)));

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
    for (int line_i = debugger::history_size; line_i < clipper.DisplayStart; line_i++) {
        cur_addr += disasm.Disassemble(emu, cur_addr);
    }

    // display only visible items
    int line_i = clipper.DisplayStart;
    int hist_i = line_i;
    for (; line_i < clipper.DisplayEnd; hist_i++) {
        uint16_t op_addr, op_cycles, num_bytes;
        bool line_valid = true;
        if (hist_i < debugger::history_size) {
            auto hist_item = board.dbg.get_history_item(hist_i);
            if (hist_item.valid) {
                op_addr = hist_item.pc;
                op_cycles = hist_item.cycles;
                num_bytes = disasm.Disassemble(emu, hist_item.pc);
                if (board.dbg.is_breakpoint(hist_item.pc)) {
                    ImGui::PushStyleColor(ImGuiCol_Text, UI::EnabledBreakpointColor);
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_Text, UI::DisabledBreakpointColor);
                }
            }
            else {
                line_valid = false;
            }
        }
        else {
            op_addr = cur_addr;
            op_cycles = 0;
            num_bytes = disasm.Disassemble(emu, op_addr);
            if (board.dbg.is_breakpoint(cur_addr)) {
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
        if (!line_valid) {
            continue;
        }
        // wrap current PC into 2 separator lines
        if ((hist_i == debugger::history_size) || (hist_i == debugger::history_size+1)) {
            ImGui::Separator();
        }

        // set breakpoint
        ImGui::PushID(line_i);
        if (ImGui::Button(" B ")) {
            board.dbg.toggle_breakpoint(op_addr);
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
            ImGui::Text("%02X ", mem_rd(&board.mem, op_addr++));
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
        line_i++;
    }
    clipper.End();
    ImGui::PopStyleVar(2);
    ImGui::EndChild();
}

} // namespace YAKC
