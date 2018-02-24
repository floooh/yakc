//------------------------------------------------------------------------------
//  MemoryWindow.cc
//------------------------------------------------------------------------------
#include "IMUI/IMUI.h"
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
read_func(uint8_t* base, size_t off) {
    uint16_t addr = uint16_t(uintptr_t(base)) + uint16_t(off);
    return mem_rd(&board.mem, addr);
}

//------------------------------------------------------------------------------
static void
write_func(uint8_t* base, size_t off, uint8_t value) {
    uint16_t addr = uint16_t(uintptr_t(base)) + uint16_t(off);
    mem_wr(&board.mem, addr, value);
}

//------------------------------------------------------------------------------
bool
MemoryWindow::Draw(yakc& emu) {
    this->edit.ReadFn = read_func;
    this->edit.WriteFn = write_func;
    ImGui::SetNextWindowSize(ImVec2(512, 256), ImGuiSetCond_Once);
    this->edit.DrawWindow(this->title.AsCStr(), 0, (1<<16));
    this->Visible = this->edit.Open;
    return this->Visible;
}

} // namespace YAKC
