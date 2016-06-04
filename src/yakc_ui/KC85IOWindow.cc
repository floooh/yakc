//------------------------------------------------------------------------------
//  KC85IOWindow.cc
//------------------------------------------------------------------------------
#include "KC85IOWindow.h"
#include "IMUI/IMUI.h"
#include "yakc_ui/UI.h"
#include "yakc/kc85.h"

using namespace Oryol;

namespace YAKC {

static const int offset = 200;

//------------------------------------------------------------------------------
void
KC85IOWindow::Setup(yakc& emu) {
    this->setName("KC85 IO Ports");
}

//------------------------------------------------------------------------------
static void
onOffLine(const char* text, bool on) {
    ImGui::Text("%s", text);
    ImGui::SameLine(offset);
    ImGui::TextColored(on?UI::EnabledColor:UI::DisabledColor, "%s", on?"ON":"OFF");
}

//------------------------------------------------------------------------------
bool
KC85IOWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(240, 384), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        if (ImGui::CollapsingHeader("PIO A (0x88)", "#kc85_io_a", true, true)) {
            const ubyte a = emu.board.pio.port[z80pio::A].output;
            onOffLine("Bit 0: CAOS ROM E", a&kc85::PIO_A_CAOS_ROM);
            onOffLine("Bit 1: RAM", a&kc85::PIO_A_RAM);
            onOffLine("Bit 2: IRM", a&kc85::PIO_A_IRM);
            onOffLine("Bit 3: RAM R/O", a&kc85::PIO_A_RAM_RO);
            onOffLine("Bit 4: unused", a&kc85::PIO_A_UNUSED);
            onOffLine("Bit 5: tape LED", a&kc85::PIO_A_TAPE_LED);
            onOffLine("Bit 6: tape motor", a&kc85::PIO_A_TAPE_MOTOR);
            onOffLine("Bit 7: BASIC ROM", a&kc85::PIO_A_BASIC_ROM);
        }
        if (ImGui::CollapsingHeader("PIO B (0x89)", "#kc85_io_b", true, true)) {
            const ubyte b = emu.board.pio.port[z80pio::B].output;
            ImGui::Text("Bit 0..5: Volume"); ImGui::SameLine(offset); ImGui::Text("%02X", b & kc85::PIO_B_VOLUME_MASK);
            if (emu.is_device(device::kc85_4)) {
                onOffLine("Bit 5: RAM8", b&kc85::PIO_B_RAM8);
                onOffLine("Bit 6: RAM8 R/O", b&kc85::PIO_B_RAM8_RO);
            }
            else {
                onOffLine("Bit 5: unused", b&(1<<5));
                onOffLine("Bit 6: unused", b&(1<<6));
            }
            onOffLine("Bit 7: blinking", b&kc85::PIO_B_BLINK_ENABLED);
        }
        if (emu.is_device(device::kc85_4)) {
            if (ImGui::CollapsingHeader("Port 0x84", "#kc85_io_84", true, true)) {
                const ubyte v = emu.kc85.io84;
                onOffLine("Bit 0: view image 0/1", v&kc85::IO84_SEL_VIEW_IMG);
                onOffLine("Bit 1: access pixel/color", v&kc85::IO84_SEL_CPU_COLOR);
                onOffLine("Bit 2: access image 0/1", v&kc85::IO84_SEL_CPU_IMG);
                onOffLine("Bit 3: lo/hi-color mode", v&kc85::IO84_HICOLOR);
                onOffLine("Bit 4: RAM8 select 0/1", v&kc85::IO84_SEL_RAM8);
                onOffLine("Bit 5: RAM8 block (?)", v&kc85::IO84_BLOCKSEL_RAM8);
                onOffLine("Bit 6: unused", v&(1<<6));
                onOffLine("Bit 7: unused", v&(1<<7));
            }
            if (ImGui::CollapsingHeader("Port 0x86", "#kc85_io_86", true, true)) {
                const ubyte v = emu.kc85.io86;
                onOffLine("Bit 0: RAM4", v&kc85::IO86_RAM4);
                onOffLine("Bit 1: RAM4 R/O", v&kc85::IO86_RAM4_RO);
                onOffLine("Bit 2: unused", v&(1<<2));
                onOffLine("Bit 3: unused", v&(1<<3));
                onOffLine("Bit 4: unused", v&(1<<4));
                onOffLine("Bit 5: unused", v&(1<<5));
                onOffLine("Bit 6: unused", v&(1<<6));
                onOffLine("Bit 7: CAOS ROM C", v&(1<<7));

            }
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
