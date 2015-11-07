//------------------------------------------------------------------------------
//  ui_cpu_window.cc
//------------------------------------------------------------------------------
#include "ui_cpu_window.h"

using namespace Oryol;
using namespace yakc;

OryolClassImpl(cpu_window);

static const z80::reg regs16[] = {
    z80::AF, z80::BC, z80::DE, z80::HL,
    z80::AF_, z80::BC_, z80::DE_, z80::HL_,
    z80::IX, z80::IY, z80::SP, z80::PC
};
static const z80::reg regs8[] = {
    z80::I, z80::R, z80::IM
};

//------------------------------------------------------------------------------
void
cpu_window::setup(const kc85& kc) {
    this->set_name("CPU");
    for (z80::reg r : regs16) {
        this->reg_widget[r].configure16(z80::reg_name(r), kc.cpu.get16(r));
    }
    for (z80::reg r : regs8) {
        this->reg_widget[r].configure8(z80::reg_name(r), kc.cpu.get8(r));
    }
}

//------------------------------------------------------------------------------
bool
cpu_window::draw(kc85& kc) {
    if (ImGui::Begin(this->title.AsCStr(), &this->visible, ImGuiWindowFlags_AlwaysAutoResize)) {

        // 16-bit registers
        int i = 0;
        ImGui::PushItemWidth(32);
        for (z80::reg r : regs16) {
            if (this->reg_widget[r].draw()) {
                kc.cpu.set16(r, this->reg_widget[r].get16());
            }
            else {
                this->reg_widget[r].set16(kc.cpu.get16(r));
            }
            if (i < 3) ImGui::SameLine(++i * 72); else i = 0;
        }
        ImGui::PopItemWidth();

        // special 8-bit registers
        ImGui::PushItemWidth(16);
        for (z80::reg r : regs8) {
            if (this->reg_widget[r].draw()) {
                kc.cpu.set8(r, this->reg_widget[r].get8());
            }
            else {
                this->reg_widget[r].set8(kc.cpu.get8(r));
            }
            if (i < 2) ImGui::SameLine(++i * 72); else i = 0;
        }
        ImGui::PopItemWidth();

        // flags
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
        ImGui::Text("Flags: %s", strFlags);
        ImGui::SameLine(144);

        // T-counter:
        ImGui::Text("T-states: %d", kc.cpu.state.T);

        // current or invalid opcode
        if (kc.cpu.state.INV) {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Invalid Opcode: 0x%02X 0x%02X 0x%02X 0x%02X",
                kc.cpu.mem.r8(kc.cpu.state.PC),
                kc.cpu.mem.r8(kc.cpu.state.PC+1),
                kc.cpu.mem.r8(kc.cpu.state.PC+2),
                kc.cpu.mem.r8(kc.cpu.state.PC+3));
        }
        else {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Next Opcode: 0x%02X 0x%02X 0x%02X 0x%02X",
            kc.cpu.mem.r8(kc.cpu.state.PC),
            kc.cpu.mem.r8(kc.cpu.state.PC+1),
            kc.cpu.mem.r8(kc.cpu.state.PC+2),
            kc.cpu.mem.r8(kc.cpu.state.PC+3));
        }

        /// FIXME: IFF, HALT, ...
        bool kc_paused = kc.paused();
        if (ImGui::Checkbox("Paused", &kc_paused)) {
            kc.pause(kc_paused);
        }
        if (kc_paused) {
            ImGui::SameLine();
            if (ImGui::Button("Step")) {
                kc.step();
            }
        }
    }
    ImGui::End();
    return this->visible;
}
