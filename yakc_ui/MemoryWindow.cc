//------------------------------------------------------------------------------
//  ui_mem_window.cc
//------------------------------------------------------------------------------
#include "MemoryWindow.h"
#include "Input/Core/Key.h"

OryolClassImpl(MemoryWindow);

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
MemoryWindow::Setup(const kc85& kc) {
    this->setName("Memory");
}

//------------------------------------------------------------------------------
bool
MemoryWindow::Draw(kc85& kc) {
    ubyte* ptr = kc.cpu.mem.get_bank_ptr(this->MemoryBankIndex);
    size_t base_display_addr = this->MemoryBankIndex * memory::bank::size;
    if (ptr) {
        this->edit.AllowEdits = kc.cpu.mem.is_bank_writable(this->MemoryBankIndex);
        ImGui::SetNextWindowSize(ImVec2(512, 96), ImGuiSetCond_Once);
        if (this->edit.Draw(this->title.AsCStr(), ptr, memory::bank::size, base_display_addr)) {
            this->Visible = true;
        }
        else {
            this->Visible = false;
        }
    }
    else {
        this->Visible = false;
    }
    return this->Visible;
}
