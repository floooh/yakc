//------------------------------------------------------------------------------
//  InfoWindow.cc
//------------------------------------------------------------------------------
#include "InfoWindow.h"
#include "IMUI/IMUI.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
InfoWindow::Setup(yakc& emu) {
    this->setName("System Info");
}

//------------------------------------------------------------------------------
bool
InfoWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(540, 440), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        ImGui::TextWrapped("%s", emu.system_info());
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC
