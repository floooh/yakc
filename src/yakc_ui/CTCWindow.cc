//------------------------------------------------------------------------------
//  CTCWindow.cc
//------------------------------------------------------------------------------
#include "CTCWindow.h"
#include "IMUI/IMUI.h"
#include "Core/String/StringBuilder.h"
#include "yakc/util/breadboard.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
CTCWindow::Setup(yakc& emu) {
    this->setName("Z80 CTC State");
}

//------------------------------------------------------------------------------
static void
drawModeBit(const yakc& emu, int chn_index, uint8_t mask, const char* name, const char* on_str, const char* off_str) {
    ImGui::Text("  %s:", name); ImGui::SameLine(128);
    if (board.z80ctc->chn[chn_index].control & mask) {
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
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        if (board.z80ctc) {
            StringBuilder strBuilder;
            for (int i = 0; i < Z80CTC_NUM_CHANNELS; i++) {
                strBuilder.Format(32, "CTC %d", i);
                if (ImGui::CollapsingHeader(strBuilder.AsCStr())) {
                    ImGui::Text("constant: %02X", board.z80ctc->chn[i].constant);
                    ImGui::Text("downcounter: %X", board.z80ctc->chn[i].down_counter);
                    ImGui::Text("int vector: %02X", board.z80ctc->chn[i].int_vector);
                    ImGui::Text("mode bits: %02X", board.z80ctc->chn[i].control);
                    drawModeBit(emu, i, Z80CTC_CTRL_EI, "INTERRUPT", "Enabled", "Disabled");
                    drawModeBit(emu, i, Z80CTC_CTRL_MODE, "MODE", "Counter", "Timer");
                    drawModeBit(emu, i, Z80CTC_CTRL_PRESCALER, "PRESCALER", "256", "16");
                    drawModeBit(emu, i, Z80CTC_CTRL_EDGE, "EDGE", "Rising", "Falling");
                    drawModeBit(emu, i, Z80CTC_CTRL_TRIGGER, "TRIGGER", "Pulse", "Automatic");
                    drawModeBit(emu, i, Z80CTC_CTRL_CONST_FOLLOWS, "CONSTANT", "Follows", "None");
                    drawModeBit(emu, i, Z80CTC_CTRL_RESET, "RESET", "Active", "Inactive");
                    drawModeBit(emu, i, Z80CTC_CTRL_CONTROL, "CONTROL", "Control-word", "Vector");
                }
            }
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
