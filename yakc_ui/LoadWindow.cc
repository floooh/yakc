//------------------------------------------------------------------------------
//  LoadWindow.cc
//------------------------------------------------------------------------------
#include "LoadWindow.h"
#include "IMUI/IMUI.h"

OryolClassImpl(LoadWIndow);

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
LoadWindow::Setup(kc85& kc) {
    this->setName("Load File");
}

//------------------------------------------------------------------------------
bool
LoadWindow::Draw(kc85& kc) {
    ImGui::SetNextWindowSize(ImVec2(256, 128), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible)) {
        ImGui::Text("FIXME!");
    }
    ImGui::End();
    return this->Visible;
}
