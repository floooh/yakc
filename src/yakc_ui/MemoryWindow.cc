//------------------------------------------------------------------------------
//  MemoryWindow.cc
//------------------------------------------------------------------------------
#include "MemoryWindow.h"
#include "Input/Input.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
MemoryWindow::Setup(yakc& emu) {
    this->setName("Memory Editor");
}

//------------------------------------------------------------------------------
static uint8_t
read_func(void* userdata, uint16_t addr) {
    return mem_rd(&board.mem, addr);
}

//------------------------------------------------------------------------------
static void
write_func(void* userdata, uint16_t addr, uint8_t value) {
    mem_wr(&board.mem, addr, value);
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
