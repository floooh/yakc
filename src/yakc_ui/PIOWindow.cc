//------------------------------------------------------------------------------
//  PIOWindow.cc
//------------------------------------------------------------------------------
#include "PIOWindow.h"
#include "IMUI/IMUI.h"
#include "yakc/z80pio.h"
#include "yakc_ui/UI.h"

using namespace Oryol;

namespace YAKC {

static const int offset = 128;

//------------------------------------------------------------------------------
void
PIOWindow::Setup(yakc& emu) {
    this->setName("Z80 PIO State");
}

//------------------------------------------------------------------------------
static void
status(const char* name, ubyte val) {
    ImGui::Text("%s", name); ImGui::SameLine(offset); ImGui::Text("%02X", val);
}

//------------------------------------------------------------------------------
static void
pioStatus(const yakc& emu, int port_id) {
    const auto& p = emu.board.pio.port[port_id];
    status("mode:", p.mode);
    status("output:", p.output);
    status("input:", p.input);
    status("io select:", p.io_select);
    status("int control:", p.int_control);
    status("int vector:", p.int_vector);
    status("int mask:", p.int_mask);
    ImGui::Text("expect:"); ImGui::SameLine(offset);
    const char* expect = "???";
    switch (p.expect) {
        case z80pio::expect_any: expect = "ANY"; break;
        case z80pio::expect_io_select: expect = "SELECT"; break;
        case z80pio::expect_int_mask: expect = "MASK"; break;
        default: break;
    }
    ImGui::Text("%s", expect);
}

//------------------------------------------------------------------------------
bool
PIOWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(220, 384), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        if (ImGui::CollapsingHeader("PIO A", "#pio_a", true, true)) {
            pioStatus(emu, z80pio::A);
        }
        if (ImGui::CollapsingHeader("PIO B", "#pio_b", true, true)) {
            pioStatus(emu, z80pio::B);
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC

