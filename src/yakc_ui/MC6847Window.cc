//------------------------------------------------------------------------------
//  MC6847Window.cc
//------------------------------------------------------------------------------
#include "MC6847Window.h"
#include "IMUI/IMUI.h"
#include "yakc_ui/UI.h"
#include "yakc/chips/mc6847.h"

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
    const mc6847& mc = emu.board.mc6847;
    ImGui::SetNextWindowSize(ImVec2(200, 292), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        on_off("HSYNC:", mc.test(mc6847::HSYNC));
        on_off("FSYNC:", mc.test(mc6847::FSYNC));
        on_off("A/G:", mc.test(mc6847::A_G));
        on_off("A/S:", mc.test(mc6847::A_S));
        on_off("INT/EXT:", mc.test(mc6847::INT_EXT));
        on_off("INV:", mc.test(mc6847::INV));
        on_off("CSS:", mc.test(mc6847::CSS));
        on_off("GM0:", mc.test(mc6847::GM0));
        on_off("GM1:", mc.test(mc6847::GM1));
        on_off("GM2:", mc.test(mc6847::GM2));
        ImGui::Text("Hori Count: %d\n", mc.h_count/mc6847::prec);
        ImGui::Text("Line Count: %d\n", mc.l_count);
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC

