//------------------------------------------------------------------------------
//  ModuleWindow.cc
//------------------------------------------------------------------------------
#include "ModuleWindow.h"
#include "IMUI/IMUI.h"
#include "yakc/systems/kc85.h"

using namespace Oryol;

namespace YAKC {

//------------------------------------------------------------------------------
void
ModuleWindow::Setup(yakc& emu) {
    this->setName("KC85 Expansion Slots");
}

//------------------------------------------------------------------------------
void
ModuleWindow::drawModuleSlot(uint8_t slot_addr) {
    ImGui::PushID(slot_addr);
    ImGui::AlignFirstTextHeightToWidgets();
    ImGui::Text("SLOT %02X:", slot_addr); ImGui::SameLine();
    const auto& mod = kc85.mod_by_slot_addr(slot_addr);
    if (ImGui::Button(mod.name, ImVec2(192, 0))) {
        ImGui::OpenPopup("select");
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", mod.desc);
    }
    ImGui::SameLine();
    ImGui::Text("type:%02X", mod.id);
    if (ImGui::BeginPopup("select")) {
        for (int i = 0; i < KC85_MODULE_NUM; i++) {
            kc85_module_type_t type = (kc85_module_type_t) i;
            if (kc85.is_module_registered(type)) {
                const auto& mod = kc85.module_template(type);
                if (ImGui::Selectable(mod.name)) {
                    if (kc85.slot_occupied(slot_addr)) {
                        kc85.remove_module(slot_addr);
                    }
                    kc85.insert_module(slot_addr, type);
                }
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

//------------------------------------------------------------------------------
bool
ModuleWindow::Draw(yakc& emu) {
    ImGui::SetNextWindowSize(ImVec2(384, 116), ImGuiSetCond_Once);
    if (ImGui::Begin(this->title.AsCStr(), &this->Visible, ImGuiWindowFlags_NoResize)) {
        this->drawModuleSlot(0x08);     // base device, right expansion slot
        this->drawModuleSlot(0x0C);     // base device, left expansion slot
        ImGui::TextWrapped("Hover over slot buttons to get help about inserted module!");
    }
    ImGui::End();
    return this->Visible;
}

} // namespace YAKC

