//------------------------------------------------------------------------------
//  ModuleWindow.cc
//------------------------------------------------------------------------------
#include "ModuleWindow.h"
#include "IMUI/IMUI.h"
#include "yakc/roms.h"

using namespace Oryol;

namespace yakc {

//------------------------------------------------------------------------------
void
ModuleWindow::Setup(emu& emu) {
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
        ImGui::SetTooltip("%s", slot.mod.desc);
    }
    ImGui::SameLine();
    ImGui::Text("type:%02X ctrl:%02X", slot.mod.id, slot.control_byte);
    if (ImGui::BeginPopup("select")) {
        for (int i = 0; i < kc85_exp::num_module_types; i++) {
            kc85_exp::module_type type = (kc85_exp::module_type)i;
            if (kc.exp.is_module_registered(type)) {
                const auto& mod = kc.exp.module_template(type);
                if (ImGui::Selectable(mod.name)) {
                    if (kc.exp.slot_occupied(slot_addr)) {
                        kc.exp.remove_module(slot_addr, kc.board->cpu.mem);
                    }
                    kc.exp.insert_module(slot_addr, type);
                    kc.board->cpu.out(slot_addr<<8|0x80, 0x00);
                }
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

//------------------------------------------------------------------------------
bool
ModuleWindow::Draw(emu& emu) {
    ImGui::SetNextWindowSize(ImVec2(384, 116), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_ShowBorders)) {
        this->drawModuleSlot(emu.kc85, 0x08);     // base device, right expansion slot
        this->drawModuleSlot(emu.kc85, 0x0C);     // base device, left expansion slot
        ImGui::TextWrapped("Hover over slot buttons to get help about inserted module!");
    }
    ImGui::End();
    return this->Visible;
}

} // namespace yakc

