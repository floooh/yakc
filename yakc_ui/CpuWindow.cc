//------------------------------------------------------------------------------
//  CpuWindow.cc
//------------------------------------------------------------------------------
#include "CpuWindow.h"
#include "Disasm.h"

using namespace Oryol;
using namespace yakc;

OryolClassImpl(CpuWindow);

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
CpuWindow::Setup(const kc85& kc) {
    this->setName("CPU");
    for (z80::reg r : regs16) {
        this->regWidget[r].Configure16(z80::reg_name(r), kc.cpu.get16(r));
    }
    for (z80::reg r : regs8) {
        this->regWidget[r].Configure8(z80::reg_name(r), kc.cpu.get8(r));
    }
}

//------------------------------------------------------------------------------
bool
CpuWindow::Draw(kc85& kc) {
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_AlwaysAutoResize)) {

        // 16-bit registers
        int i = 0;
        ImGui::PushItemWidth(32);
        for (z80::reg r : regs16) {
            if (this->regWidget[r].Draw()) {
                kc.cpu.set16(r, this->regWidget[r].Get16());
            }
            else {
                this->regWidget[r].Set16(kc.cpu.get16(r));
            }
            if (i < 3) ImGui::SameLine(++i * 72); else i = 0;
        }
        ImGui::PopItemWidth();

        // special 8-bit registers
        ImGui::PushItemWidth(16);
        for (z80::reg r : regs8) {
            if (this->regWidget[r].Draw()) {
                kc.cpu.set8(r, this->regWidget[r].Get8());
            }
            else {
                this->regWidget[r].Set8(kc.cpu.get8(r));
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

        // IFF1 and IFF2
        const ImVec4 red(1.0f, 0.0f, 0.0f, 1.0f);
        const ImVec4 green(0.0f, 1.0f, 0.0f, 1.0f);
        ImGui::TextColored(kc.cpu.state.IFF1 ? green:red, "IFF1"); ImGui::SameLine();
        ImGui::TextColored(kc.cpu.state.IFF2 ? green:red, "IFF2");

        // current or invalid opcode
        if (kc.cpu.state.INV) {
            ImGui::TextColored(red, "Invalid Opcode: 0x%02X 0x%02X 0x%02X 0x%02X",
                kc.cpu.mem.r8(kc.cpu.state.PC),
                kc.cpu.mem.r8(kc.cpu.state.PC+1),
                kc.cpu.mem.r8(kc.cpu.state.PC+2),
                kc.cpu.mem.r8(kc.cpu.state.PC+3));
        }
        else {
            Disasm disasm;
            disasm.Disassemble(kc, kc.cpu.state.PC);
            ImGui::TextColored(green, "Next Instruction: %s", disasm.Result());
        }

        /// FIXME: IFF, HALT, ...
        bool kc_paused = kc.paused();
        if (ImGui::Checkbox("Halt", &kc_paused)) {
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
    return this->Visible;
}
