//------------------------------------------------------------------------------
//  MemoryWindow.cc
//------------------------------------------------------------------------------
#include "MemoryWindow.h"
#include "Input/Core/Key.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
MemoryWindow::Setup(yakc& emu) {
    this->setName("Memory Editor");
}

//------------------------------------------------------------------------------
static ubyte
read_func(void* userdata, uword addr) {
    yakc* emu = (yakc*) userdata;
    return emu->board.cpu.mem.r8(addr);
}

//------------------------------------------------------------------------------
static void
write_func(void* userdata, uword addr, ubyte value) {
    yakc* emu = (yakc*) userdata;
    emu->board.cpu.mem.w8(addr, value);
}

//------------------------------------------------------------------------------
bool
MemoryWindow::Draw(yakc& emu) {
    this->edit.AllowEdits = true;
    ImGui::SetNextWindowSize(ImVec2(512, 256), ImGuiSetCond_Once);
    if (this->edit.Draw(this->title.AsCStr(), read_func, write_func, &emu)) {
        this->Visible = true;
    }
    else {
        this->Visible = false;
    }
    return this->Visible;
}

} // namespace YAKC
