//------------------------------------------------------------------------------
//  PIOWindow.cc
//------------------------------------------------------------------------------
#include "PIOWindow.h"
#include "IMUI/IMUI.h"
#include "yakc/devices/z80pio.h"
#include "yakc_ui/UI.h"

using namespace Oryol;

namespace YAKC {

static const int offset = 128;

//------------------------------------------------------------------------------
PIOWindow::PIOWindow(const char* name, z80pio* pio) :
Name(name),
PIO(pio) {
    // empty
}


//------------------------------------------------------------------------------
void
PIOWindow::Setup(yakc& emu) {
    o_assert(this->Name && this->PIO);
    this->setName(this->Name);
}

//------------------------------------------------------------------------------
static void
status(const char* name, ubyte val) {
    ImGui::Text("%s", name); ImGui::SameLine(float(offset)); ImGui::Text("%02X", val);
}

//------------------------------------------------------------------------------
static const char*
modeAsString(ubyte m) {
    switch (m) {
        case 0: return "output";
        case 1: return "input";
        case 2: return "bidirectional";
        case 3: return "bitcontrol";
        default: return "invalid";
    };
}

//------------------------------------------------------------------------------
static void
pioStatus(z80pio* pio, int port_id) {
    const auto& p = pio->port[port_id];
    status("mode:", p.mode); ImGui::SameLine(); ImGui::Text("%s", modeAsString(p.mode));
    status("output:", p.output);
    status("input:", p.input);
    status("io select:", p.io_select);
    status("int control:", p.int_control);
    ImGui::Text("    int enabled: %s", (p.int_control & 0x80) ? "ENABLED":"DISABLED");
    ImGui::Text("    and/or:      %s", (p.int_control & 0x40) ? "AND":"OR");
    ImGui::Text("    high/low:    %s", (p.int_control & 0x20) ? "HIGH":"LOW");
    status("int vector:", p.int_vector);
    status("int mask:", p.int_mask);
    ImGui::Text("expect:"); ImGui::SameLine(float(offset));
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
    o_assert(this->PIO);
    ImGui::SetNextWindowSize(ImVec2(220, 420), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        if (ImGui::CollapsingHeader("PIO A", "#pio_a", true, true)) {
            pioStatus(this->PIO, z80pio::A);
        }
        if (ImGui::CollapsingHeader("PIO B", "#pio_b", true, true)) {
            pioStatus(this->PIO, z80pio::B);
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC

