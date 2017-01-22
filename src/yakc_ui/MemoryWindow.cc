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
    if (emu->cpu_type() == cpu::z80) {
        return emu->board.z80cpu.mem.r8(addr);
    }
    else {
        return emu->board.m6502cpu.mem.r8(addr);
    }
}

//------------------------------------------------------------------------------
static void
write_func(void* userdata, uword addr, ubyte value) {
    yakc* emu = (yakc*) userdata;
    if (emu->cpu_type() == cpu::z80) {
        emu->board.z80cpu.mem.w8(addr, value);
    }
    else {
        emu->board.m6502cpu.mem.w8(addr, value);
    }
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
