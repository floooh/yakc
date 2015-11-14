//------------------------------------------------------------------------------
//  ModuleWindow.cc
//------------------------------------------------------------------------------
#include "ModuleWindow.h"
#include "IMUI/IMUI.h"
#include "yakc_roms/roms.h"

OryolClassImpl(ModuleWindow);

using namespace Oryol;
using namespace yakc;

//------------------------------------------------------------------------------
void
ModuleWindow::Setup(kc85& kc) {
    this->setupModules(kc);
    this->setName("Expansion Modules");
}

//------------------------------------------------------------------------------
void
ModuleWindow::drawModuleSlot(kc85& kc, ubyte slot_addr) {
    ImGui::PushID(slot_addr);
    ImGui::AlignFirstTextHeightToWidgets();
    ImGui::Text("SLOT %02X:", slot_addr); ImGui::SameLine();
    const kc85::module_slot& slot = kc.get_module_slot(slot_addr);
    if (ImGui::Button(slot.desc.name, ImVec2(192, 0))) {
        ImGui::OpenPopup("select");
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(slot.desc.help);
    }
    ImGui::SameLine();
    ImGui::Text("type:%02X ctrl:%02X", slot.desc.type, slot.control_byte);
    if (ImGui::BeginPopup("select")) {
        for (const auto& mod : this->modules) {
            if (ImGui::Selectable(mod.name)) {
                kc.insert_module(slot_addr, mod);
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

//------------------------------------------------------------------------------
bool
ModuleWindow::Draw(kc85& kc) {
    ImGui::SetNextWindowSize(ImVec2(384, 116), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_NoResize)) {
        this->drawModuleSlot(kc, 0x08);     // base device, right expansion slot
        this->drawModuleSlot(kc, 0x0C);     // base device, left expansion slot
        ImGui::TextWrapped("Hover over slot buttons to get help about inserted module!");
    }
    ImGui::End();
    return this->Visible;
}

//------------------------------------------------------------------------------
void
ModuleWindow::setupModules(kc85& kc) {

    kc85::module_desc mod;

    // empty module slot
    mod.help = "Click to select module!";
    kc.insert_module(0x08, mod);    // just set the help text...
    kc.insert_module(0x0C, mod);    // just set the help text...
    this->modules.Add(mod);

    // M022 EXPANDER RAM
    mod.name = "M022 EXPANDER-RAM (16KB)";
    mod.help = "A 16 KByte RAM expansion module.\n\n"
        "SWITCH [SLOT] 43: map to address 0x4000\n"
        "SWITCH [SLOT] 83: map to address 0x8000\n"
        "SWITCH [SLOT] 00: switch module off\n\n"
        "...where [SLOT] is 08 or 0C";
    mod.type = 0xF4;
    mod.writable = true;
    mod.size = 0x4000;
    this->modules.Add(mod);

    // M026 FORTH
    YAKC_ASSERT(sizeof(rom_forth) == 0x2000);
    mod.name = "M026 FORTH";
    mod.help = "FORTH language expansion module.\n\n"
        "First deactivate the BASIC ROM with:\n"
        "SWITCH 02 00\n\n"
        "Then activate FORTH with:\n"
        "SWITCH [SLOT] C1\n\n"
        "...where [SLOT] is 08 or 0C";
    mod.type = 0xFB;
    mod.writable = false;
    mod.size = 0x2000;
    mod.ptr = rom_forth;
    this->modules.Add(mod);

    // M027 DEVELOPMENT
    YAKC_ASSERT(sizeof(rom_develop) == 0x2000);
    mod.name = "M027 DEVELOPMENT";
    mod.help = "Assembler/disassembler expansion module.\n\n"
        "First deactivate the BASIC ROM with:\n"
        "SWITCH 02 00\n\n"
        "Then activate the module with:\n"
        "SWITCH [SLOT] C1\n\n"
        "...where [SLOT] is 08 or 0C";
    mod.type = 0xFB;
    mod.writable = false;
    mod.size = 0x2000;
    mod.ptr = rom_develop;
    this->modules.Add(mod);

}