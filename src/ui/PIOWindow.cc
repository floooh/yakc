//------------------------------------------------------------------------------
//  PIOWindow.cc
//------------------------------------------------------------------------------
#include "PIOWindow.h"
#include "IMUI/IMUI.h"
#include "core/z80pio.h"
#include "ui/UI.h"

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
PIOWindow::Setup(kc85& kc) {
    this->setName("Z80 PIO State");
    this->pioAData.Configure8("PIO A Data (port 0x88)", kc.pio.read(z80pio::A));
    this->pioBData.Configure8("PIO B Data (port 0x89)", kc.pio.read(z80pio::B));
}

//------------------------------------------------------------------------------
static void
onOffLine(const char* text, bool on) {
    ImGui::Text("%s", text);
    ImGui::SameLine(96);
    ImGui::TextColored(on?UI::EnabledColor:UI::DisabledColor, "%s", on?"ON":"OFF");
}

//------------------------------------------------------------------------------
static void
pioControlState(const kc85& kc, z80pio::channel chn) {
    const auto& state = kc.pio.channel_state[chn];
    const bool en = state.interrupt_enabled;
    ImGui::Text("Interrupt:");
    ImGui::SameLine(128);
    ImGui::TextColored(en?UI::EnabledColor:UI::DisabledColor, en?"ENABLED":"DISABLED");

    ImGui::Text("Interrupt Vector:");
    ImGui::SameLine(128);
    ImGui::Text("%02X", state.interrupt_vector);

    ImGui::Text("Mode:");
    ImGui::SameLine(128);
    ImGui::Text("%02X", state.mode);
}

//------------------------------------------------------------------------------
bool
PIOWindow::Draw(kc85& kc) {
    ImGui::SetNextWindowSize(ImVec2(220, 384), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        ImGui::BeginChild("##child");
        if (this->pioAData.Draw()) {
            kc.cpu.out(0x88, this->pioAData.Get8());
        }
        else {
            this->pioAData.Set8(kc.pio.read(z80pio::A));
        }
        const ubyte a = kc.pio.read(z80pio::A);
        onOffLine("CAOS ROM:", 0 != (a & kc85::PIO_A_CAOS_ROM));
        onOffLine("RAM:", 0 != (a & kc85::PIO_A_RAM));
        onOffLine("IRM:", 0 != (a & kc85::PIO_A_IRM));
        onOffLine("RAM R/O:", 0 != (a & kc85::PIO_A_RAM_RO));
        onOffLine("TAPE LED:", 0 != (a & kc85::PIO_A_TAPE_LED));
        onOffLine("TAPE MOTOR:", 0 != (a & kc85::PIO_A_TAPE_MOTOR));
        onOffLine("BASIC ROM:", 0 != (a & kc85::PIO_A_BASIC_ROM));
        ImGui::Separator();
        
        if (this->pioBData.Draw()) {
            kc.cpu.out(0x89, this->pioBData.Get8());
        }
        else {
            this->pioBData.Set8(kc.pio.read(z80pio::B));
        }
        const ubyte b = kc.pio.read(z80pio::B);
        ImGui::Text("VOLUME:"); ImGui::SameLine(96); ImGui::Text("%02X", b & kc85::PIO_B_VOLUME_MASK);
        onOffLine("BLINKING", 0 != (b & kc85::PIO_B_BLINK_ENABLED));
        ImGui::Separator();
        ImGui::Text("PIO A Control (port 0x8A):");
        pioControlState(kc, z80pio::A);
        ImGui::Separator();
        ImGui::Text("PIO B Control (port 0x8B):");
        pioControlState(kc, z80pio::B);
        ImGui::EndChild();
    }
    ImGui::End();
    return this->Visible;
}
