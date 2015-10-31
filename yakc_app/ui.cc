//------------------------------------------------------------------------------
//  ui.cc
//------------------------------------------------------------------------------
#include "ui.h"
#include "Core/Containers/StaticArray.h"
#include "IMUI/IMUI.h"
#include "Time/Clock.h"
#include "Input/Input.h"

using namespace Oryol;
using namespace yakc;

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
char
ui::nibble_to_str(ubyte n) {
    return "0123456789ABCDEF"[n & 0xF];
}

//------------------------------------------------------------------------------
void
ui::ubyte_to_str(ubyte b, char* buf, int buf_size) {
    o_assert(buf_size >= 3);
    buf[0] = nibble_to_str(b>>4);
    buf[1] = nibble_to_str(b);
    buf[2] = 0;
}

//------------------------------------------------------------------------------
void
ui::uword_to_str(uword w, char* buf, int buf_size) {
    o_assert(buf_size >= 5);
    ubyte_to_str(w>>8, buf, buf_size); buf+=2; buf_size-=2;
    ubyte_to_str(w, buf, buf_size);
}

//------------------------------------------------------------------------------
uword
ui::parse_uword(const char* str, uword old_val) {
    // parse 4 hex characters into a
    uword res = 0;
    for (int i = 0; i < 4; i++) {
        int shift = (3-i)*4;
        char c = str[i];
        if ((c >= '0') && (c <= '9')) {
            res |= (c-'0') << shift;
        }
        else if ((c >= 'A') && (c <= 'F')) {
            res |= ((c-'A')+10) << shift;
        }
        else {
            // failure
            return old_val;
        }
    }
    return res;
}

//------------------------------------------------------------------------------
void
ui::regs_to_buf(const kc85& kc) {
    for (int i = 0; i < reg16::num; i++) {
        this->reg_to_buf(kc, (reg16::r16)i);
    }
}

//------------------------------------------------------------------------------
void
ui::reg_to_buf(const kc85& kc, reg16::r16 r) {
    // read cpu state to text buffers
    const auto& regs = kc.cpu.state;
    uword val = 0;
    switch (r) {
        case reg16::AF: val = regs.AF; break;
        case reg16::BC: val = regs.BC; break;
        case reg16::DE: val = regs.DE; break;
        case reg16::HL: val = regs.HL; break;
        case reg16::AF_: val = regs.AF_; break;
        case reg16::BC_: val = regs.BC_; break;
        case reg16::DE_: val = regs.DE_; break;
        case reg16::HL_: val = regs.HL_; break;
        case reg16::IX: val = regs.IX; break;
        case reg16::IY: val = regs.IY; break;
        case reg16::SP: val = regs.SP; break;
        case reg16::PC: val = regs.PC; break;
        default: break;
    }
    uword_to_str(val, this->r16[r].buf, reg16::buf_size);
}

//------------------------------------------------------------------------------
void
ui::buf_to_reg(kc85& kc, reg16::r16 r) const {
    auto& regs = kc.cpu.state;
    uword* val = &regs.AF;
    switch (r) {
        case reg16::AF: val = &regs.AF; break;
        case reg16::BC: val = &regs.BC; break;
        case reg16::DE: val = &regs.DE; break;
        case reg16::HL: val = &regs.HL; break;
        case reg16::AF_: val = &regs.AF_; break;
        case reg16::BC_: val = &regs.BC_; break;
        case reg16::DE_: val = &regs.DE_; break;
        case reg16::HL_: val = &regs.HL_; break;
        case reg16::IX: val = &regs.IX; break;
        case reg16::IY: val = &regs.IY; break;
        case reg16::SP: val = &regs.SP; break;
        case reg16::PC: val = &regs.PC; break;
        default: break;
    }
    *val = parse_uword(this->r16[r].buf, *val);
}

//------------------------------------------------------------------------------
void
ui::cpu(kc85& kc) {
    // read register values into text buffers
    ImGui::Begin("U880", &this->show_cpu, ImGuiWindowFlags_AlwaysAutoResize);

    // 16-bit registers
    ImGui::PushItemWidth(32);
    int editFlags = ImGuiInputTextFlags_CharsHexadecimal|ImGuiInputTextFlags_CharsUppercase|ImGuiInputTextFlags_EnterReturnsTrue;
    for (int i = 0; i < reg16::num; i++) {
        const reg16::r16 r = (reg16::r16) i;
        if (ImGui::InputText(reg16::name(r), this->r16[r].buf, reg16::buf_size, editFlags)) {
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




