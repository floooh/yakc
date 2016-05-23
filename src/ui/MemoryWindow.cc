//------------------------------------------------------------------------------
//  MemoryWindow.cc
//------------------------------------------------------------------------------
#include "MemoryWindow.h"
#include "Input/Core/Key.h"

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
MemoryWindow::Setup(kc85& kc) {
    this->setName("Memory Editor");
}

//------------------------------------------------------------------------------
static ubyte
read_func(void* userdata, uword addr) {
    kc85* kc = (kc85*) userdata;
    return kc->board->cpu.mem.r8(addr);
}

//------------------------------------------------------------------------------
static void
write_func(void* userdata, uword addr, ubyte value) {
    kc85* kc = (kc85*) userdata;
    kc->board->cpu.mem.w8(addr, value);
}

//------------------------------------------------------------------------------
bool
MemoryWindow::Draw(kc85& kc) {
    this->edit.AllowEdits = true;
    ImGui::SetNextWindowSize(ImVec2(512, 256), ImGuiSetCond_Once);
    if (this->edit.Draw(this->title.AsCStr(), read_func, write_func, &kc)) {
        this->Visible = true;
    }
    else {
        this->Visible = false;
    }
    return this->Visible;
}
