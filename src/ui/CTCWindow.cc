//------------------------------------------------------------------------------
//  CTCWindow.cc
//------------------------------------------------------------------------------
#include "CTCWindow.h"
#include "IMUI/IMUI.h"
#include "Core/String/StringBuilder.h"

OryolClassImpl(CTCWindow);

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
CTCWindow::Setup(kc85& kc) {
    this->setName("Z80 CTC State");
}

//------------------------------------------------------------------------------
static void
drawModeBit(const kc85& kc, int chn_index, ubyte mask, const char* name, const char* on_str, const char* off_str) {
    ImGui::Text("  %s:", name); ImGui::SameLine(128);
    if (kc.ctc.channels[chn_index].mode & mask) {
        ImGui::Text("%s", on_str);
    }
    else {
        ImGui::Text("%s", off_str);
    }
}


//------------------------------------------------------------------------------
bool
CTCWindow::Draw(kc85& kc) {
    ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        StringBuilder strBuilder;
        for (int i = 0; i < z80ctc::num_channels; i++) {
            strBuilder.Format(32, "Channel %d (port 0x%02X)", i, i+0x8C);
            if (ImGui::CollapsingHeader(strBuilder.AsCStr())) {
                ImGui::Text("Time Constant: %02X", kc.ctc.channels[i].constant);
                ImGui::Text("Interrupt Vector: %02X", kc.ctc.channels[i].interrupt_vector);
                ImGui::Text("Mode Bits: %02X", kc.ctc.channels[i].mode);
                drawModeBit(kc, i, z80ctc::INTERRUPT, "INTERRUPT", "Enabled", "Disabled");
                drawModeBit(kc, i, z80ctc::MODE, "MODE", "Counter", "Timer");
                drawModeBit(kc, i, z80ctc::PRESCALER, "PRESCALER", "256", "16");
                drawModeBit(kc, i, z80ctc::EDGE, "EDGE", "Rising", "Falling");
                drawModeBit(kc, i, z80ctc::TRIGGER, "TRIGGER", "Pulse", "Automatic");
                drawModeBit(kc, i, z80ctc::CONSTANT_FOLLOWS, "CONSTANT", "Follows", "None");
                drawModeBit(kc, i, z80ctc::RESET, "RESET", "Active", "Inactive");
                drawModeBit(kc, i, z80ctc::CONTROL, "CONTROL", "Control-word", "Vector");
            }
        }
    }
    ImGui::End();
    return this->Visible;
}

