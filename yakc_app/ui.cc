//------------------------------------------------------------------------------
//  ui.cc
//------------------------------------------------------------------------------
#include "ui.h"
#include "util.h"
#include "Core/Containers/StaticArray.h"
#include "IMUI/IMUI.h"
#include "Time/Clock.h"
#include "Input/Input.h"

using namespace Oryol;
using namespace yakc;

static const z80::reg regs16[] = {
    z80::AF, z80::BC, z80::DE, z80::HL,
    z80::AF_, z80::BC_, z80::DE_, z80::HL_,
    z80::IX, z80::IY, z80::SP, z80::PC
};

//------------------------------------------------------------------------------
ui::ui() :
show_cpu(true),
kc_paused(false) {
    // empty
}

//------------------------------------------------------------------------------
void
ui::setup(kc85& kc) {
    IMUI::Setup();
    this->curTime = Clock::Now();
    this->kc_paused = kc.paused();
    this->regs_to_buf(kc);
}

//------------------------------------------------------------------------------
void
ui::discard() {
    IMUI::Discard();
}

//------------------------------------------------------------------------------
void
ui::onframe(kc85& kc) {
    IMUI::NewFrame(Clock::LapTime(this->curTime));
    this->cpu(kc);
    ImGui::Render();
}

//------------------------------------------------------------------------------
void
ui::regs_to_buf(const kc85& kc) {
    for (z80::reg r : regs16) {
        this->reg_to_buf(kc, r);
    }
}

//------------------------------------------------------------------------------
void
ui::reg_to_buf(const kc85& kc, z80::reg r) {
    // read cpu state to text buffers
    util::uword_to_str(kc.cpu.get16(r), this->buf[r], buf_size);
}

//------------------------------------------------------------------------------
void
ui::buf_to_reg(kc85& kc, z80::reg r) const {
    kc.cpu.set16(r, util::parse_uword(this->buf[r], kc.cpu.get16(r)));
}

//------------------------------------------------------------------------------
void
ui::cpu(kc85& kc) {
    // read register values into text buffers
    ImGui::Begin("U880", &this->show_cpu, ImGuiWindowFlags_AlwaysAutoResize);

    // 16-bit registers
    ImGui::PushItemWidth(32);
    int editFlags = ImGuiInputTextFlags_CharsHexadecimal|ImGuiInputTextFlags_CharsUppercase|ImGuiInputTextFlags_EnterReturnsTrue;
    for (z80::reg r : regs16) {
        if (ImGui::InputText(z80::reg_name(r), this->buf[r], buf_size, editFlags)) {
            this->buf_to_reg(kc, r);
        }
        else {
            this->reg_to_buf(kc, r);
        }
        if (int c = (r+1)%4) {
            ImGui::SameLine(c*72);
        }
    }
    ImGui::PopItemWidth();

    // flags
    StaticArray<char,9> strFlags;
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
    ImGui::Text("flags: %s", strFlags.begin());

    if (ImGui::Checkbox("Paused", &this->kc_paused)) {
        kc.pause(this->kc_paused);
    }
    if (kc.paused()) {
        ImGui::SameLine();
        if (ImGui::Button("Step")) {
            kc.step();
        }
    }
    ImGui::End();
}




