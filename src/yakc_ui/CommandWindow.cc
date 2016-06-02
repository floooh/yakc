//------------------------------------------------------------------------------
//  CommandWindow.cc
//------------------------------------------------------------------------------
#include "CommandWindow.h"
#include "IMUI/IMUI.h"
#include "Core/String/StringBuilder.h"
#include "yakc_ui/UI.h"
#include <ctype.h>

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
CommandWindow::Setup(yakc& emu) {
    this->setName("Find Commands");
    this->prologByteWidget.Configure8("Prolog Byte", 0x7F);
}

//------------------------------------------------------------------------------
bool
CommandWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(200, 250), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        this->prologByteWidget.Draw();
        ImGui::SameLine();
        if (ImGui::Button("Scan...")) {
            this->scan(emu, this->prologByteWidget.Get8());
        }
        for (int i = 0; i < this->commands.Size(); i++) {
            const Cmd& cmd = this->commands[i];
            if (emu.board.dbg.is_breakpoint(cmd.addr)) {
                ImGui::PushStyleColor(ImGuiCol_Text, UI::EnabledBreakpointColor);
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, UI::DisabledBreakpointColor);
            }
            ImGui::PushID(i);
            if (ImGui::Button(" B ")) {
                emu.board.dbg.toggle_breakpoint(0, cmd.addr);
            }
            ImGui::PopID();
            ImGui::SameLine();
            ImGui::Text("0x%04X %s", cmd.addr, cmd.name.AsCStr());
            ImGui::PopStyleColor();
        }
    }
    ImGui::End();
    return this->Visible;
}

//------------------------------------------------------------------------------
void
CommandWindow::scan(const yakc& emu, ubyte prologByte) {
    StringBuilder strBuilder;

    this->commands.Clear();
    ubyte prevByte = emu.board.cpu.mem.r8(0x0000);
    for (unsigned int addr = 0x0001; addr < 0x10000; addr++) {
        const ubyte curByte = emu.board.cpu.mem.r8(addr);
        if ((curByte == prologByte) && (prevByte == prologByte)) {
            // found a header, scan for 00 or 01 byte
            addr++;
            ubyte c;
            while (isalnum(c = emu.board.cpu.mem.r8(addr++))) {
                strBuilder.Append(c);
            }
            // if it was a valid command, add it to commands array
            if ((c == 0) || (c == 1)) {
                this->commands.Add(strBuilder.GetString(), addr);
            }
            strBuilder.Clear();
        }
        prevByte = curByte;
    }
}

} // namespace YAKC

