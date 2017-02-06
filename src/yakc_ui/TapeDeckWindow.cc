//------------------------------------------------------------------------------
//  TapeDeckWindow.cc
//------------------------------------------------------------------------------
#include "TapeDeckWindow.h"
#include "IMUI/IMUI.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
TapeDeckWindow::Setup(yakc& emu) {
    this->setName("GERACORD 2000");
}

//------------------------------------------------------------------------------
bool
TapeDeckWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(200, 240), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {

    }
    ImGui::End();
    return this->Visible;
}


} // namespace YAKC
