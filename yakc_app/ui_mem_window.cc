//------------------------------------------------------------------------------
//  ui_mem_window.cc
//------------------------------------------------------------------------------
#include "ui_mem_window.h"

OryolClassImpl(mem_window);

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
mem_window::setup(const kc85& kc) {
    this->set_name("Memory");
    this->start_widget.configure16("##start", 0x0200);
    this->len_widget.configure16("##end", 0x0400);
    this->update_membuffer(kc);
}

//------------------------------------------------------------------------------
bool
mem_window::draw(kc85& kc) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(line_len*7 + 32, 96.0f));
    if (ImGui::Begin(this->title.AsCStr(), &this->visible, ImGuiWindowFlags_NoScrollbar)) {
        ImGui::PushItemWidth(32);
        bool start_changed = this->start_widget.draw();
        ImGui::SameLine(); ImGui::Text("-"); ImGui::SameLine();
        bool len_changed = this->len_widget.draw();
        ImGui::PopItemWidth();
        if (start_changed | len_changed) {
            this->update_membuffer(kc);
        }
        ImGui::InputTextMultiline("##mem", this->buf, sizeof(this->buf), ImVec2(line_len*7 + 14, -1.0f));
    }
    ImGui::End();
    ImGui::PopStyleVar();
    return this->visible;
}

//------------------------------------------------------------------------------
void
mem_window::update_membuffer(const kc85& kc) {
    int addr = this->start_widget.get16();
    int end_addr = addr + this->len_widget.get16();
    char* dst_ptr = this->buf;
    char* dst_end = dst_ptr + sizeof(this->buf);
    while (addr < end_addr) {
        o_assert(dst_ptr < (dst_end - line_len));
        uword uw_addr = (uword) addr;

        // '1234: '
        util::uword_to_str(uw_addr, dst_ptr, 5);
        dst_ptr += 4;
        *dst_ptr++ = ':';
        *dst_ptr++ = ' ';

        // 16 bytes
        for (uword i = 0; i < 16; i++) {
            util::ubyte_to_str(kc.cpu.mem.r8(uw_addr+i), dst_ptr, 3);
            dst_ptr += 2;
            *dst_ptr++ = ' ';
        }

        // ' |', 16 ASCII values, '|'
        *dst_ptr++ = '|';
        for (uword i = 0; i < 16; i++) {
            ubyte c = kc.cpu.mem.r8(uw_addr+i);
            if ((c < 32) | (c > 127)) {
                c = '.';
            }
            *dst_ptr++ = c;
        }
        *dst_ptr++ = '|';
        *dst_ptr++ = '\n';

        addr += 16;
        o_assert(dst_ptr < dst_end);
    }
    *dst_ptr++ = 0;
    o_assert(dst_ptr < dst_end);
}
