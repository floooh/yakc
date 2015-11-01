//------------------------------------------------------------------------------
//  ui_mem_window.cc
//------------------------------------------------------------------------------
#include "ui_mem_window.h"
#include "Input/Core/Key.h"

OryolClassImpl(mem_window);

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
mem_window::setup(const kc85& kc) {
    this->set_name("Memory");
}

//------------------------------------------------------------------------------
bool
mem_window::draw(kc85& kc) {
    ubyte* ptr = kc.cpu.mem.get_bank_ptr(this->memory_bank_index);
    size_t base_display_addr = this->memory_bank_index * memory::bank::size;
    if (ptr) {
        this->edit.AllowEdits = kc.cpu.mem.is_bank_writable(this->memory_bank_index);
        ImGui::SetNextWindowSize(ImVec2(512, 96), ImGuiSetCond_Once);
        if (this->edit.Draw(this->title.AsCStr(), ptr, memory::bank::size, base_display_addr)) {
            this->visible = true;
        }
        else {
            this->visible = false;
        }
    }
    else {
        this->visible = false;
    }
    return this->visible;
}
