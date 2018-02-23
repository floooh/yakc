//------------------------------------------------------------------------------
//  MC6847Window.cc
//------------------------------------------------------------------------------
#include "MC6847Window.h"
#include "IMUI/IMUI.h"
#include "yakc_ui/UI.h"

using namespace Oryol;

namespace YAKC {

static const int offset = 80;

//------------------------------------------------------------------------------
void
MC6847Window::Setup(yakc& emu) {
    this->setName("MC6847 State");
}

//------------------------------------------------------------------------------
static void
on_off(const char* text, bool on) {
    ImGui::Text("%s", text);
    ImGui::SameLine(float(offset));
    ImGui::TextColored(on?UI::EnabledColor:UI::DisabledColor, "%s", on?"ON":"OFF");
}

//------------------------------------------------------------------------------
bool
MC6847Window::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(200, 292), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        const mc6847_t& mc = board.mc6847;
        on_off("HSYNC:", mc.pins & MC6847_HS);
        on_off("FSYNC:", mc.pins & MC6847_FS);
        on_off("A/G:", mc.pins & MC6847_AG);
        on_off("A/S:", mc.pins & MC6847_AS);
        on_off("INT/EXT:", mc.pins & MC6847_INTEXT);
        on_off("INV:", mc.pins & MC6847_INV);
        on_off("CSS:", mc.pins & MC6847_CSS);
        on_off("GM0:", mc.pins & MC6847_GM0);
        on_off("GM1:", mc.pins & MC6847_GM1);
        on_off("GM2:", mc.pins & MC6847_GM2);
        ImGui::Text("Hori Count: %d\n", mc.h_count / MC6847_PRECISION_BOOST);
        ImGui::Text("Line Count: %d\n", mc.l_count);
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC

