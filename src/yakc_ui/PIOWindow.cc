//------------------------------------------------------------------------------
//  PIOWindow.cc
//------------------------------------------------------------------------------
#include "PIOWindow.h"
#include "IMUI/IMUI.h"
#include "yakc_ui/UI.h"

using namespace Oryol;

namespace YAKC {

static const int offset = 128;

//------------------------------------------------------------------------------
PIOWindow::PIOWindow(const char* name, z80pio_t* pio) :
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
status(const char* name, uint8_t val) {
    ImGui::Text("%s", name); ImGui::SameLine(float(offset)); ImGui::Text("%02X", val);
}

//------------------------------------------------------------------------------
static const char*
modeAsString(uint8_t m) {
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
pioStatus(z80pio_t* pio, int port_id) {
    const auto& p = pio->port[port_id];
    status("mode:", p.mode); ImGui::SameLine(); ImGui::Text("%s", modeAsString(p.mode));
    status("output:", p.output);
    status("input:", p.input);
    status("io select:", p.io_select);
    status("int control:", p.int_control);
    ImGui::Text("    int enabled: %s", (p.int_control & Z80PIO_INTCTRL_EI) ? "ENABLED":"DISABLED");
    ImGui::Text("    and/or:      %s", (p.int_control & Z80PIO_INTCTRL_ANDOR) ? "AND":"OR");
    ImGui::Text("    high/low:    %s", (p.int_control & Z80PIO_INTCTRL_HILO) ? "HIGH":"LOW");
    status("int vector:", p.int_vector);
    status("int mask:", p.int_mask);
    ImGui::Text("expect:"); ImGui::SameLine(float(offset));
    const char* expect = "???";
    if (p.expect_int_mask) expect = "MASK";
    else if (p.expect_io_select) expect = "IOSELECT";
    else expect = "ANY";
    ImGui::Text("%s", expect);
}

//------------------------------------------------------------------------------
bool
PIOWindow::Draw(yakc& emu) {
    o_assert(this->PIO);
    ImGui::SetNextWindowSize(ImVec2(220, 420), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        if (ImGui::CollapsingHeader("PIO A", "#pio_a", true, true)) {
            pioStatus(this->PIO, Z80PIO_PORT_A);
        }
        if (ImGui::CollapsingHeader("PIO B", "#pio_b", true, true)) {
            pioStatus(this->PIO, Z80PIO_PORT_B);
        }
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC

