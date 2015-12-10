//------------------------------------------------------------------------------
//  CommandWindow.cc
//------------------------------------------------------------------------------
#include "CommandWindow.h"
#include "IMUI/IMUI.h"
#include "Core/String/StringBuilder.h"

OryolClassImpl(CommandWindow);

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
CommandWindow::Setup(kc85& kc) {
    this->setName("Find Commands");
    this->prologByteWidget.Configure8("Prolog Byte", 0x7F);
}

//------------------------------------------------------------------------------
bool
CommandWindow::Draw(kc85& kc) {
    ImGui::SetNextWindowSize(ImVec2(200, 250), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_ShowBorders)) {
        ImGui::PushItemWidth(16);
        this->prologByteWidget.Draw();
        ImGui::SameLine();
        if (ImGui::Button("Scan...")) {
            this->scan(kc, this->prologByteWidget.Get8());
        }
        for (int i = 0; i < this->commands.Size(); i++) {
            const Cmd& cmd = this->commands[i];
            if (kc.dbg.is_breakpoint(cmd.addr)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 1.0f, 1.0f));
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
            ImGui::PushID(i);
            if (ImGui::Button(" B ")) {
                kc.dbg.toggle_breakpoint(0, cmd.addr);
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
CommandWindow::scan(const kc85& kc, ubyte prologByte) {
    StringBuilder strBuilder;

    this->commands.Clear();
    ubyte prevByte = kc.cpu.mem.r8(0x0000);
    for (unsigned int addr = 0x0001; addr < 0x10000; addr++) {
        const ubyte curByte = kc.cpu.mem.r8(addr);
        if ((curByte == prologByte) && (prevByte == prologByte)) {
            // found a header, scan for 00 or 01 byte
            addr++;
            ubyte c;
            while (isalnum(c = kc.cpu.mem.r8(addr++))) {
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
