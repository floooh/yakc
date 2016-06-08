//------------------------------------------------------------------------------
//  CTCWindow.cc
//------------------------------------------------------------------------------
#include "CTCWindow.h"
#include "IMUI/IMUI.h"
#include "Core/String/StringBuilder.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
CTCWindow::Setup(yakc& emu) {
    this->setName("Z80 CTC State");
}

//------------------------------------------------------------------------------
static void
drawModeBit(const yakc& emu, int chn_index, ubyte mask, const char* name, const char* on_str, const char* off_str) {
    ImGui::Text("  %s:", name); ImGui::SameLine(128);
    if (emu.board.ctc.channels[chn_index].mode & mask) {
        ImGui::Text("%s", on_str);
    }
    else {
        ImGui::Text("%s", off_str);
    }
}


//------------------------------------------------------------------------------
bool
CTCWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        StringBuilder strBuilder;
        for (int i = 0; i < z80ctc::num_channels; i++) {
            strBuilder.Format(32, "CTC %d", i);
            if (ImGui::CollapsingHeader(strBuilder.AsCStr())) {
                ImGui::Text("constant: %02X", emu.board.ctc.channels[i].constant);
                ImGui::Text("downcounter: %X", emu.board.ctc.channels[i].down_counter);
                ImGui::Text("int vector: %02X", emu.board.ctc.channels[i].interrupt_vector);
                ImGui::Text("mode bits: %02X", emu.board.ctc.channels[i].mode);
                drawModeBit(emu, i, z80ctc::INTERRUPT, "INTERRUPT", "Enabled", "Disabled");
                drawModeBit(emu, i, z80ctc::MODE, "MODE", "Counter", "Timer");
                drawModeBit(emu, i, z80ctc::PRESCALER, "PRESCALER", "256", "16");
                drawModeBit(emu, i, z80ctc::EDGE, "EDGE", "Rising", "Falling");
                drawModeBit(emu, i, z80ctc::TRIGGER, "TRIGGER", "Pulse", "Automatic");
                drawModeBit(emu, i, z80ctc::CONSTANT_FOLLOWS, "CONSTANT", "Follows", "None");
                drawModeBit(emu, i, z80ctc::RESET, "RESET", "Active", "Inactive");
                drawModeBit(emu, i, z80ctc::CONTROL, "CONTROL", "Control-word", "Vector");
            }
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
