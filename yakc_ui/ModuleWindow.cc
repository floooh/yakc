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
    this->setName("Expansion Slots");
}

//------------------------------------------------------------------------------
void
ModuleWindow::drawModuleSlot(kc85& kc, ubyte slot_addr) {
    ImGui::PushID(slot_addr);
    ImGui::AlignFirstTextHeightToWidgets();
    ImGui::Text("SLOT %02X:", slot_addr); ImGui::SameLine();
    const auto& slot = kc.exp.slot_by_addr(slot_addr);
    if (ImGui::Button(slot.mod.name, ImVec2(192, 0))) {
        ImGui::OpenPopup("select");
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(slot.mod.help);
    }
    ImGui::SameLine();
    ImGui::Text("type:%02X ctrl:%02X", slot.mod.type, slot.control_byte);
    if (ImGui::BeginPopup("select")) {
        for (const auto& mod : this->modules) {
            if (ImGui::Selectable(mod.name)) {
                if (kc.exp.slot_occupied(slot_addr)) {
                    kc.exp.remove_module(slot_addr, kc.cpu.mem);
                }
                kc.exp.insert_module(slot_addr, mod);
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

    // empty module slot
    auto mod = kc85_module::create_empty("NO MODULE", "Click to insert module!");
    if (!kc.exp.slot_occupied(0x08)) {
        kc.exp.slot_by_addr(0x08).mod = mod;
    }
    if (!kc.exp.slot_occupied(0x0C)) {
        kc.exp.slot_by_addr(0x0C).mod = mod;
    }
    this->modules.Add(mod);

    // M022 EXPANDER RAM
    this->modules.Add(kc85_module::create_ram(0xF4, 0x4000, "M022 EPANDER RAM",
        "16 KByte RAM expansion module.\n\n"
        "SWITCH [SLOT] 43: map to address 0x4000\n"
        "SWITCH [SLOT] 83: map to address 0x8000\n"
        "SWITCH [SLOT] 00: switch module off\n\n"
        "...where [SLOT] is 08 or 0C"));

    // M011 64 K RAM
    this->modules.Add(kc85_module::create_ram(0xF6, 0x10000, "M011 64 KBYTE RAM",
        "64 KByte RAM expansion module.\n\n"
        "SWITCH [SLOT] 03: map 1st block to 0x0000\n"
        "SWITCH [SLOT] 43: map 1st block to 0x4000\n"
        "SWITCH [SLOT] 83: map 1st block to 0x8000\n"
        "SWITCH [SLOT] C3: map 1st block to 0xC000\n"
        "...where [SLOT] is 08 or 0C.\n"));

    // M026 FORTH
    this->modules.Add(kc85_module::create_rom(0xFB, rom_forth, 0x2000, "M026 FORTH",
        "FORTH language expansion module.\n\n"
        "First deactivate the BASIC ROM with:\n"
        "SWITCH 02 00\n\n"
        "Then activate FORTH with:\n"
        "SWITCH [SLOT] C1\n\n"
        "...where [SLOT] is 08 or 0C"));

    // M027 DEVELOPMENT
    this->modules.Add(kc85_module::create_rom(0xFB, rom_develop, 0x2000, "M027 DEVELOPMENT",
        "Assembler/disassembler expansion module.\n\n"
        "First deactivate the BASIC ROM with:\n"
        "SWITCH 02 00\n\n"
        "Then activate the module with:\n"
        "SWITCH [SLOT] C1\n\n"
        "...where [SLOT] is 08 or 0C"));
}